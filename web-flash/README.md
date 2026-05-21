# Reppo 网页固件烧录

面向小白用户的 **ESP32-S3 N16R8** 浏览器烧录页，适配 Reppo R1 产品（`project-name` 工程）。

## 界面

- 全屏粒子连线背景（`#0a0a0a` + 青蓝粒子 `#00d9ff`）
- 半透明遮罩提升文字可读性
- 三步说明 + 一键「开始烧录固件」

## 使用前准备

1. 编译固件（项目根目录）：

   ```bash
   idf.py build
   ```

2. 复制 bin 到网页目录：

   **Windows**

   ```powershell
   .\web-flash\scripts\prepare-firmware.ps1
   ```

   **Linux / macOS**

   ```bash
   chmod +x web-flash/scripts/prepare-firmware.sh
   ./web-flash/scripts/prepare-firmware.sh
   ```

3. 本地预览（须 **HTTPS** 或 **localhost**，且使用 Chrome / Edge）：

   **重要：`http://localhost:8080` 不能直接在浏览器里打开，必须先启动下面的服务器。**

   **Windows 推荐（双击即可）：**

   ```
   双击 web-flash/start-server.bat
   ```

   会弹出黑色命令行窗口并自动打开浏览器。**不要关闭该窗口**，关掉后网页就打不开了。

   或手动启动：

   ```powershell
   cd web-flash
   python -m http.server 8080
   ```

   然后再用浏览器访问：`http://localhost:8080`

   **不要** 双击 `index.html` 用文件方式打开（`file://` 无法烧录）。

## 上线部署

将 `web-flash/` 整个目录部署到 **HTTPS** 静态站点（GitHub Pages、Cloudflare Pages、自有 Nginx 等）。

`firmware/` 目录内需包含 `manifest.json` 及所有 `.bin` 文件；`prepare-firmware` 脚本会从 `build/` 自动拷贝。

## 烧录分区说明

| 文件 | 偏移 |
|------|------|
| bootloader.bin | 0x0 |
| partition-table.bin | 0x8000 |
| ota_data_initial.bin | 0xd000 |
| project-name.bin | 0x10000 |
| storage.bin | 0xa10000 |

与 `partitions.csv` 及 `idf.py build` 输出一致。

## 用户操作

1. USB 连接设备  
2. 必要时按住 BOOT 再插线  
3. 点击「开始烧录固件」→ 选择 USB 串口 → 等待完成  

## 技术栈

- [ESP Web Tools](https://github.com/esphome/esp-web-tools)（Web Serial + esptool-js）
- 自定义 Canvas 粒子背景

## 注意

- Safari / Firefox 不支持 Web Serial，请用 **Chrome** 或 **Edge**
- 固件 `.bin` 体积较大，不建议提交到 Git；部署前运行准备脚本即可
