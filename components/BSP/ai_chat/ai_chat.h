#ifndef AI_CHAT_H
#define AI_CHAT_H

#include "esp_err.h"
#include <stddef.h>

// ========== AI API 配置 ==========
// 支持任何 OpenAI 兼容格式的 API（DeepSeek / 通义千问 / OpenAI 等）
// 请将下方的 API Key 替换为您自己的密钥

#define AI_DEFAULT_URL        "https://api.deepseek.com/chat/completions"
#define AI_DEFAULT_MODEL      "deepseek-v4-pro"
#define AI_DEFAULT_MAX_TOKENS 800
#define AI_RESPONSE_BUF_SIZE  4096
#define AI_REQUEST_BUF_SIZE   2048
#define AI_API_TIMEOUT_MS     30000

/**
 * @brief 初始化 AI 聊天模块
 * @param api_url  API 端点 URL（传 NULL 使用默认 DeepSeek URL）
 * @param api_key  您的 API 密钥
 * @param model    模型名称（传 NULL 使用默认模型）
 * @return ESP_OK on success
 */
esp_err_t ai_chat_init(const char *api_url, const char *api_key, const char *model);

/**
 * @brief 发送消息并获取 AI 回复（阻塞调用，约 3-30 秒）
 * @param user_message  用户输入的消息
 * @param response_buf  接收回复的缓冲区
 * @param buf_len       缓冲区大小
 * @return ESP_OK on success
 */
esp_err_t ai_chat_send(const char *user_message, char *response_buf, size_t buf_len);

#endif /* AI_CHAT_H */
