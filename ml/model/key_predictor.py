import re
from collections import defaultdict
import Levenshtein  # Thư viện để tính khoảng cách Levenshtein
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.naive_bayes import MultinomialNB
import numpy as np
import joblib  # Thư viện để lưu/tải mô hình
import os

class TypoCorrector:
    def __init__(self, typo_dict=None):
        """
        Khởi tạo mô hình sửa lỗi gõ phím.
        Args:
            typo_dict (dict): Dictionary chứa các cặp {từ sai: từ đúng}.
        """
        self.typo_dict = typo_dict if typo_dict else {}
        self.vectorizer = CountVectorizer(analyzer='char', ngram_range=(2, 3))
        self.model = MultinomialNB()
        self.trained = False
        self.word_freq = defaultdict(int)  # Lưu tần suất từ để ưu tiên từ phổ biến

    def train(self, additional_data=None):
        """
        Huấn luyện mô hình dựa trên dictionary và dữ liệu bổ sung.
        Args:
            additional_data (list of tuples): Danh sách các cặp (từ sai, từ đúng) bổ sung.
        """
        # Kết hợp dictionary và dữ liệu bổ sung
        training_data = list(self.typo_dict.items())
        if additional_data:
            training_data.extend(additional_data)

        # Tách từ sai và từ đúng
        typos, corrections = zip(*training_data)

        # Tính tần suất từ đúng để ưu tiên trong dự đoán
        for correct in corrections:
            self.word_freq[correct] += 1

        # Vector hóa các từ sai để huấn luyện mô hình
        X = self.vectorizer.fit_transform(typos)
        y = corrections

        # Huấn luyện mô hình Naive Bayes
        self.model.fit(X, y)
        self.trained = True

    def save_model(self, directory="model"):
        """
        Lưu mô hình đã huấn luyện, vectorizer, typo_dict và word_freq vào thư mục chỉ định.
        Args:
            directory (str): Thư mục để lưu các tệp mô hình.
        """
        if not self.trained:
            raise ValueError("Mô hình chưa được huấn luyện. Vui lòng gọi train() trước khi lưu.")

        # Tạo thư mục nếu chưa tồn tại
        os.makedirs(directory, exist_ok=True)

        # Lưu mô hình Naive Bayes
        joblib.dump(self.model, os.path.join(directory, "naive_bayes_model.pkl"))

        # Lưu vectorizer
        joblib.dump(self.vectorizer, os.path.join(directory, "vectorizer.pkl"))

        # Lưu typo_dict
        joblib.dump(self.typo_dict, os.path.join(directory, "typo_dict.pkl"))

        # Lưu word_freq
        joblib.dump(self.word_freq, os.path.join(directory, "word_freq.pkl"))

        print(f"Mô hình đã được lưu vào thư mục {directory}")

    def load_model(self, directory="model"):
        """
        Tải mô hình, vectorizer, typo_dict và word_freq từ thư mục chỉ định.
        Args:
            directory (str): Thư mục chứa các tệp mô hình.
        """
        try:
            # Tải mô hình Naive Bayes
            self.model = joblib.load(os.path.join(directory, "naive_bayes_model.pkl"))

            # Tải vectorizer
            self.vectorizer = joblib.load(os.path.join(directory, "vectorizer.pkl"))

            # Tải typo_dict
            self.typo_dict = joblib.load(os.path.join(directory, "typo_dict.pkl"))

            # Tải word_freq
            self.word_freq = joblib.load(os.path.join(directory, "word_freq.pkl"))

            self.trained = True
            print(f"Mô hình đã được tải từ thư mục {directory}")
        except FileNotFoundError as e:
            raise FileNotFoundError(f"Không tìm thấy tệp mô hình trong thư mục {directory}: {e}")

    def correct(self, word, context=None, max_suggestions=3):
        """
        Sửa lỗi gõ phím cho một từ.
        Args:
            word (str): Từ cần sửa.
            context (str, optional): Câu chứa từ để hỗ trợ sửa theo ngữ cảnh.
            max_suggestions (int): Số lượng gợi ý tối đa.
        Returns:
            list: Danh sách các từ đúng được gợi ý, kèm theo độ tin cậy.
        """
        # Kiểm tra trực tiếp trong dictionary
        if word in self.typo_dict:
            return [(self.typo_dict[word], 1.0)]

        # Nếu không có trong dictionary, dùng mô hình hoặc Levenshtein
        suggestions = []

        if self.trained:
            # Dự đoán bằng mô hình Naive Bayes
            X = self.vectorizer.transform([word])
            probs = self.model.predict_proba(X)[0]
            predicted_labels = self.model.classes_
            top_indices = np.argsort(probs)[-max_suggestions:][::-1]
            suggestions = [(predicted_labels[i], probs[i]) for i in top_indices]

        # Kết hợp với tìm kiếm Levenshtein để tăng độ chính xác
        levenshtein_suggestions = self._levenshtein_search(word, max_suggestions)
        suggestions.extend(levenshtein_suggestions)

        # Sắp xếp và ưu tiên từ phổ biến
        suggestions = sorted(
            set(suggestions),  # Loại bỏ trùng lặp
            key=lambda x: (x[1] * self.word_freq.get(x[0], 1), x[1]),
            reverse=True
        )[:max_suggestions]

        # Nếu có ngữ cảnh, có thể tích hợp thêm xử lý NLP ở đây (hiện chưa triển khai)
        if context:
            # Placeholder cho xử lý ngữ cảnh (dùng BERT hoặc ngôn ngữ khác nếu cần)
            pass

        return suggestions if suggestions else [(word, 1.0)]  # Trả về từ gốc nếu không tìm thấy

    def _levenshtein_search(self, word, max_suggestions):
        """
        Tìm các từ gần giống bằng Levenshtein distance.
        Args:
            word (str): Từ cần sửa.
            max_suggestions (int): Số lượng gợi ý tối đa.
        Returns:
            list: Danh sách (từ, độ tương đồng).
        """
        suggestions = []
        for correct_word in self.typo_dict.values():
            distance = Levenshtein.distance(word, correct_word)
            similarity = 1 / (1 + distance)  # Đơn giản hóa độ tương đồng
            suggestions.append((correct_word, similarity))
        return sorted(suggestions, key=lambda x: x[1], reverse=True)[:max_suggestions]

    def add_typo(self, typo, correction):
        """
        Thêm một cặp từ sai/từ đúng mới vào dictionary.
        Args:
            typo (str): Từ sai.
            correction (str): Từ đúng.
        """
        self.typo_dict[typo] = correction
        self.word_freq[correction] += 1
        # Cần huấn luyện lại mô hình nếu thêm dữ liệu mới
        self.trained = False

    def correct_sentence(self, sentence, max_suggestions=1):
        """
        Sửa lỗi gõ phím cho cả câu.
        Args:
            sentence (str): Câu cần sửa.
            max_suggestions (int): Số gợi ý tối đa cho mỗi từ.
        Returns:
            str: Câu đã được sửa.
        """
        words = re.findall(r'\b\w+\b', sentence)
        corrected_words = []
        
        for word in words:
            suggestions = self.correct(word, max_suggestions=max_suggestions)
            corrected_words.append(suggestions[0][0])  # Chọn gợi ý tốt nhất

        # Tái tạo câu
        corrected_sentence = sentence
        for original, corrected in zip(words, corrected_words):
            corrected_sentence = re.sub(r'\b' + original + r'\b', corrected, corrected_sentence)
        
        return corrected_sentence

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