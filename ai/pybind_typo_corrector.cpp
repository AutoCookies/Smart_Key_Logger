#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

namespace py = pybind11;

class PyTypoCorrector {
public:
    PyTypoCorrector(const std::string& model_path) {
        py::gil_scoped_acquire acquire;
        py_module = py::module_::import("TypoCorrector");
        py_corrector = py_module.attr("TypoCorrector")();
        py_corrector.attr("load_model")(model_path);
    }

    std::string correct(const std::string& input) {
        py::gil_scoped_acquire acquire;
        auto result = py_corrector.attr("correct")(input);
        auto suggestions = result.cast<std::vector<std::tuple<std::string, float>>>();
        return suggestions.empty() ? input : suggestions[0].get<0>();
    }

    std::string correct_sentence(const std::string& sentence) {
        py::gil_scoped_acquire acquire;
        return py_corrector.attr("correct_sentence")(sentence).cast<std::string>();
    }

private:
    py::module_ py_module;
    py::object py_corrector;
};

PYBIND11_MODULE(typo_corrector, m) {
    py::class_<PyTypoCorrector>(m, "PyTypoCorrector")
        .def(py::init<std::string>())
        .def("correct", &PyTypoCorrector::correct)
        .def("correct_sentence", &PyTypoCorrector::correct_sentence);
}