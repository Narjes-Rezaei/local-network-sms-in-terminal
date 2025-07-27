# Server class description

```
#include <iostream>      // برای استفاده از cout و cin
#include <string>        // برای استفاده از std::string
#include <vector>        // برای استفاده از std::vector
#include <stdexcept>     // برای پرتاب خطا (مثلاً std::runtime_error)
#include <unistd.h>      // برای توابع سیستمی مثل close، read، write، gethostname
#include <arpa/inet.h>   // برای تبدیل آدرس‌ها به فرمت شبکه (inet_ntoa، htons و ...)
#include <netdb.h>       // برای کار با hostname و IP (gethostbyname)
#include <cstring>       // برای memset و کارهای رشته‌ای سطح پایین
```

```
private:
    int serverSocket = -1;         // سوکت سرور
    int clientSocket = -1;         // سوکت کلاینت که به سرور وصل می‌شود
    sockaddr_in serverAddr{}, clientAddr{}; // ساختارهای مربوط به آدرس IP و پورت
    socklen_t clientLen{};         // طول آدرس کلاینت برای accept
    const int PORT = 8080;         // پورت ثابت سرور
```

```
public:
Server() {
    // ساخت سوکت TCP
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
        throw std::runtime_error("Error creating socket");
```

**تعریف یک کلاس به نام Server.**
socket(AF_INET, SOCK_STREAM, 0) → ساخت یک سوکت TCP با استفاده از IPv4

اگر ساختن سوکت با شکست مواجه شود، با throw خطا ایجاد می‌کند.

```
    // تنظیمات آدرس سرور
    serverAddr.sin_family = AF_INET;               // استفاده از IPv4
    serverAddr.sin_addr.s_addr = INADDR_ANY;       // به همه آدرس‌ها گوش بده
    serverAddr.sin_port = htons(PORT);             // تبدیل پورت از host به network byte order
```

```
    // اتصال سوکت به پورت
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0)
        throw std::runtime_error("Error binding");
    std::cout << "serverSocket is created and connected" << endl;
}
```

bind اتصال آدرس و پورت به سوکت را انجام می‌دهد.
اگر شکست بخورد، خطا پرتاب می‌شود.

در غیر اینصورت پیغام :""

```
void startListening() {
    if (listen(serverSocket, 1) < 0)
        throw std::runtime_error("Error listening");
```

سوکت را در حالت گوش دادن (listen) قرار می‌دهد تا بتواند کلاینت را بپذیرد.

```
    std::cout << "listening on port: " << PORT << "..." << endl;
    clientLen = sizeof(clientAddr);
    clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
    if (clientSocket < 0)
        throw std::runtime_error("Error accepting");
    std::cout << "client connected" << endl;
}
```

accept منتظر اتصال کلاینت می‌ماند و پس از اتصال، یک سوکت جدید clientSocket را برمی‌گرداند.
اگر خطا رخ دهد، throw می‌کند.

```
std::string getOwnIP() {
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0)
        return "UnknownHost";
```

gethostname نام دستگاه را برمی‌گرداند.

```
    hostent* host = gethostbyname(hostname);
    if (!host) return "UnknownIP";
```

gethostbyname آدرس IP را برای hostname برمی‌گرداند.

```
    return std::string(inet_ntoa(*reinterpret_cast<in_addr*>(host->h_addr_list[0])));
}
```

inet_ntoa تبدیل IP از فرم باینری به رشته قابل خواندن (مثل 192.168.1.1)

```
void sendMessage(const std::string& msg) {
    if (send(clientSocket, msg.c_str(), msg.size(), 0) == -1)
        std::cerr << "Error sending message" << endl;
}
```

ارسال داده به کلاینت از طریق سوکت با استفاده از تابع send

```
std::vector<char> buffer(1024, 0);
ssize_t bytesReceived = recv(clientSocket, buffer.data(), buffer.size(), 0);
if (bytesReceived <= 0) {
    return "";
}
return std::string(buffer.data(), bytesReceived);
```

دریافت پیام از کلاینت.
اگر تعداد بایت دریافتی صفر یا منفی باشد، اتصال قطع شده یا خطا رخ داده است

.پیام دریافتی را به رشته تبدیل می‌کند

```
void closeConnection() {
    if (clientSocket != -1) close(clientSocket);
    if (serverSocket != -1) close(serverSocket);
    std::cout << "connection closed" << endl;
}
```

بستن سوکت‌ها اگر هنوز باز باشند.
