#include "hook.h"
#include "recorder.h"
#include <windows.h>
#include <string>
#include <sstream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "pybind_typo_corrector.h"

PyTypoCorrector* corrector = nullptr;
bool correctorLoaded = false;

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

            // 2. Tải mô hình nếu chưa tải
            if (!correctorLoaded)
            {
                try {
                    corrector = new PyTypoCorrector("typo_model");
                    correctorLoaded = true;
                }
                catch (const std::exception& e) {
                    std::wstring error = L"Không thể tải mô hình Python: " + std::wstring(e.what(), e.what() + strlen(e.what()));
                    MessageBoxW(nullptr, error.c_str(), L"Lỗi", MB_ICONERROR);
                    return CallNextHookEx(nullptr, nCode, wParam, lParam);
                }
            }

            // 3. Tạo chuỗi hiện tại
            static std::string currentInput;
            currentInput += keyStr;

            // 4. Gợi ý nếu phát hiện lỗi
            if (currentInput.length() >= 2)
            {
                std::string correction = corrector->correct(currentInput);
                if (!correction.empty() && correction != currentInput)
                {
                    std::wstring message = L"Có thể bạn muốn gõ: " + std::wstring(correction.begin(), correction.end());
                    MessageBoxW(nullptr, message.c_str(), L"Gợi ý chính tả", MB_ICONINFORMATION | MB_OK);
                    currentInput.clear(); // Xóa chuỗi sau khi gợi ý
                }
            }

            // Xóa chuỗi nếu gặp phím cách hoặc enter
            if (vkCode == VK_SPACE || vkCode == VK_RETURN)
            {
                currentInput.clear();
            }
        }
        else if (wParam == WM_KEYUP || wParam == WM_SYSKEYUP)
        {
            // Có thể thêm logic cho keyup nếu cần
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void SetKeyboardHook()
{
    // Khởi tạo Python runtime
    Py_Initialize();

    HHOOK hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
    if (!hHook)
    {
        MessageBoxW(nullptr, L"Không thể cài hook!", L"Lỗi", MB_ICONERROR);
        Py_Finalize();
        return;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
    delete corrector;
    Py_Finalize();
}