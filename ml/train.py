from typo_correction_dict import typo_correction_dict
from model.key_predictor import TypoCorrector

# Ví dụ sử dụng
if __name__ == "__main__":
    from typo_correction_dict import typo_correction_dict

    # Khởi tạo và huấn luyện mô hình
    corrector = TypoCorrector(typo_correction_dict)
    corrector.train()

    # Lưu mô hình
    corrector.save_model("typo_model")

    # Tải lại mô hình
    new_corrector = TypoCorrector()
    new_corrector.load_model("typo_model")

    # Kiểm tra sửa lỗi
    print(new_corrector.correct("gti"))  # Dự đoán: [('git', ...)]
    print(new_corrector.correct("pyhton"))  # Dự đoán: [('python', ...)]
    print(new_corrector.correct_sentence("I use gti to psuh my cod to teh reposiotry"))  
    # Dự đoán: "I use git to push my code to the repository"