#include <charconv>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>

#include "rkllm.h"

namespace {

LLMHandle model_handle = nullptr;

struct Options {
    std::filesystem::path language_model;
    int max_new_tokens = 256;
    int context_length = 4096;
};

void print_usage(const char* program) {
    std::cerr << "Usage: " << program
              << " --language MODEL.rkllm [--max-new-tokens 256]"
                 " [--context-length 4096]\n";
}

bool parse_positive_int(const char* value, int& output) {
    const std::string text(value);
    const auto result = std::from_chars(text.data(), text.data() + text.size(), output);
    return result.ec == std::errc() && result.ptr == text.data() + text.size() && output > 0;
}

bool parse_args(int argc, char** argv, Options& options) {
    for (int index = 1; index < argc; ++index) {
        const std::string arg(argv[index]);
        auto next_value = [&]() -> const char* {
            return ++index < argc ? argv[index] : nullptr;
        };
        if (arg == "--language") {
            const char* value = next_value();
            if (!value) return false;
            options.language_model = value;
        } else if (arg == "--max-new-tokens") {
            const char* value = next_value();
            if (!value || !parse_positive_int(value, options.max_new_tokens)) return false;
        } else if (arg == "--context-length") {
            const char* value = next_value();
            if (!value || !parse_positive_int(value, options.context_length)) return false;
        } else if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            std::exit(0);
        } else {
            return false;
        }
    }
    return !options.language_model.empty();
}

int result_callback(RKLLMResult* result, void*, LLMCallState state) {
    if (state == RKLLM_RUN_NORMAL && result && result->text) {
        std::cout << result->text << std::flush;
    } else if (state == RKLLM_RUN_FINISH) {
        std::cout << '\n';
    } else if (state == RKLLM_RUN_ERROR) {
        std::cerr << "\nRKLLM inference failed.\n";
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    Options options;
    if (!parse_args(argc, argv, options)) {
        print_usage(argv[0]);
        return 2;
    }
    if (!std::filesystem::is_regular_file(options.language_model)) {
        std::cerr << "Language model does not exist or is not a regular file: "
                  << options.language_model << '\n';
        return 2;
    }

    RKLLMParam parameters = rkllm_createDefaultParam();
    const std::string model_path = options.language_model.string();
    parameters.model_path = model_path.c_str();
    parameters.top_k = 1;
    parameters.max_new_tokens = options.max_new_tokens;
    parameters.max_context_len = options.context_length;
    parameters.skip_special_token = true;
    parameters.extend_param.base_domain_id = 1;

    RKLLMCallback callback = {};
    callback.result_callback = result_callback;
    const int init_result = rkllm_init(&model_handle, &parameters, &callback);
    if (init_result != 0 || !model_handle) {
        std::cerr << "rkllm_init failed with code " << init_result << '\n';
        return 1;
    }

    const int template_result = rkllm_set_chat_template(
        model_handle,
        "<|im_start|>system\nYou are a helpful assistant.<|im_end|>\n",
        "<|im_start|>user\n",
        "<|im_end|>\n<|im_start|>assistant\n");
    if (template_result != 0) {
        std::cerr << "rkllm_set_chat_template failed with code " << template_result << '\n';
        rkllm_destroy(model_handle);
        return 1;
    }

    RKLLMInferParam inference = {};
    inference.mode = RKLLM_INFER_GENERATE;
    inference.keep_history = 1;

    std::cout << "Offline Qwen3-VL-2B text session ready.\n"
                 "Commands: /clear clears history; /exit exits.\n";
    for (std::string prompt; std::cout << "\nUser: " && std::getline(std::cin, prompt);) {
        if (prompt == "/exit" || prompt == "exit") break;
        if (prompt == "/clear" || prompt == "clear") {
            const int clear_result = rkllm_clear_kv_cache(model_handle, 1, nullptr, nullptr);
            if (clear_result != 0) {
                std::cerr << "Failed to clear history, code " << clear_result << '\n';
            } else {
                std::cout << "Conversation history cleared.\n";
            }
            continue;
        }
        if (prompt.empty()) continue;

        RKLLMInput input = {};
        input.input_type = RKLLM_INPUT_PROMPT;
        input.role = "user";
        input.prompt_input = prompt.data();
        std::cout << "Assistant: " << std::flush;
        const int run_result = rkllm_run(model_handle, &input, &inference, nullptr);
        if (run_result != 0) {
            std::cerr << "rkllm_run failed with code " << run_result << '\n';
        }
    }

    rkllm_destroy(model_handle);
    model_handle = nullptr;
    return 0;
}
