#ifndef AI_CHAT_SECRETS_H
#define AI_CHAT_SECRETS_H

/* 1=经 ECS 代理；0=设备直连 DeepSeek（勿用于量产） */
#define AI_CHAT_USE_PROXY 1

#if AI_CHAT_USE_PROXY
/* 备案域名 + Nginx HTTPS（见 server/ai_proxy/README.md 第六节） */
#define AI_PROXY_URL    "https://reppo.top/v1/ai/summary"
/* 与服务器 .env 里 DEVICE_TOKEN 保持一致 */
#define AI_DEVICE_TOKEN "reppo-dev-token-change-me"
#else
#define AI_CHAT_API_KEY "sk-your-key-here"
#endif

#endif /* AI_CHAT_SECRETS_H */
