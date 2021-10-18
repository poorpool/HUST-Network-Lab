package net.yxchen.lab1;


import java.io.File;
import java.io.FileInputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;

public class Response {
    private OutputStream outputStream;
    private int statusCode;  // 返回的 HTTP 状态码
    private File willFile;  // 期待的返回文件

    public Response(OutputStream outputStream) {
        this.outputStream = outputStream;
    }

    public void constructResponse(Request request) {
        // parse 失败和非 get 都是 400 bad request
        if (!request.isParseSuccess()) {
            statusCode = 400;
            return;
        } else if (!"get".equalsIgnoreCase(request.getMethod())) {
            statusCode = 400;
            return;
        }

        // 文件不存在和不是文件都是 404
        Path willPath = Paths.get(ServerUtils.getBasePath(), Paths.get(request.getUri()).normalize().toString()).normalize();
        willFile = willPath.toFile();
        System.out.println("Want " + willPath);
        if (willFile.isDirectory()) {  // 目录默认为 index.html
            willFile = Paths.get(willPath.toString(), "index.html").normalize().toFile();
            System.out.println("...But it is a directory, try " + willFile.toString());
        }
        if (!willFile.exists() || !willFile.isFile()) {
            statusCode = 404;
            return;
        }

        statusCode = 200;
    }

    public void fillResponse() {
        String responseString;
        try {
            switch (statusCode) {
                case 200:
                    byte[] bytes = new byte[1024];
                    FileInputStream fileInputStream = new FileInputStream(willFile);
                    responseString = "HTTP/1.1 200 OK\r\n"
                            + "Connection: close\r\n"
                            + "Content-type: " + Files.probeContentType(willFile.toPath()) + "\r\n"
                            + "Content-Length: " + fileInputStream.available() + "\r\n"
                            + "\r\n";
                    outputStream.write(responseString.getBytes());
                    int length;
                    while ((length = fileInputStream.read(bytes)) > 0) {
                        outputStream.write(bytes, 0, length);
                    }
                    System.out.println("Get file ok, type " + Files.probeContentType(willFile.toPath()));
                    break;
                case 404:
                    responseString = "HTTP/1.1 404 File Not Found\r\n"
                            + "Content-Type: text/html\r\n"
                            + "Content-Length: 27\r\n"
                            + "\r\n"
                            + "<h1>404 File Not Found</h1>";
                    outputStream.write(responseString.getBytes());
                    System.out.println("File does not exist");
                    break;
                default:
                    responseString = "HTTP/1.1 400 Bad Request\r\n"
                            + "Content-Type: text/html\r\n"
                            + "Content-Length: 24\r\n"
                            + "\r\n"
                            + "<h1>400 Bad Request</h1>";
                    System.out.println("Bad request for file");
                    outputStream.write(responseString.getBytes());
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
