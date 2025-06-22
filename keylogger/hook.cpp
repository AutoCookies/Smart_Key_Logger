#include "hook.h"
#include "recorder.h"
#include <windows.h>
#include <string>
#include <sstream>
#include <iostream>

#include "../ai/ai_model.h"
#include "../ai/spelling_corrector.h"

AIModel model;
SpellingCorrector spellCorrector;
bool dictionaryLoaded = false;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        KBDLLHOOKSTRUCT *pKeyboard = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);

        if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)
        {
            DWORD vkCode = pKeyboard->vkCode;
            std::string keyStr;

            // Ghi lại chữ hoặc số
            if ((vkCode >= '0' && vkCode <= '9') || (vkCode >= 'A' && vkCode <= 'Z'))
            {
                keyStr = static_cast<char>(vkCode);
            }
            else
            {
                keyStr = "[VK_" + std::to_string(vkCode) + "]";
            }

            // 1. Ghi log
            LogKey(keyStr);

            // 2. Cập nhật AI model
            model.observeKey(keyStr);

            // 3. Tải từ điển nếu chưa tải
            if (!dictionaryLoaded)
            {
                spellCorrector.loadDictionary("data/dictionary.txt");
                dictionaryLoaded = true;
            }

            // 4. Tạo chuỗi hiện tại
            std::ostringstream oss;
            for (const auto &k : model.getRecentKeys())
            {
                oss << k;
            }
            std::string currentInput = oss.str();

            // 5. Gợi ý nếu phát hiện lỗi
            std::string aiSuggestion;
            if (!model.isLikelyMistake(aiSuggestion) && currentInput.length() >= 2)
            {
                std::string correction = spellCorrector.suggestCorrection(currentInput);
                if (!correction.empty() && correction != currentInput)
                {
                    std::cout << "[Gợi ý chính tả] Có thể bạn muốn gõ: " << correction << std::endl;
                }
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void SetKeyboardHook()
{
    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
    if (!hHook)
    {
        MessageBoxW(nullptr, L"Không thể cài hook!", L"Lỗi", MB_ICONERROR);
        return;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
}
