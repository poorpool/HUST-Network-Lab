package net.yxchen.lab1;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URLDecoder;
import java.nio.charset.StandardCharsets;
import java.util.Date;

public class Request {
    private InputStream inputStream;

    private boolean parseSuccess;
    private String method; // 请求方法
    private String uri;
    private String httpVersion; // 请求协议

    public Request(InputStream inputStream) {
        this.inputStream = inputStream;
    }

    /**
     * 解析请求并打印
     * @param ip 请求来源 ip
     * @param port 请求来源 port
     */
    public void parse(String ip, int port) {
        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(inputStream));
        String line = null;
        try {
            line = bufferedReader.readLine();
            if (line == null) {
                parseSuccess = false;
                return;
            }
            String [] gets = line.split(" ");
            if (gets.length != 3) {
                parseSuccess = false;
                return;
            }
            method = gets[0];
            uri = URLDecoder.decode(gets[1], StandardCharsets.UTF_8);
            httpVersion = gets[2];
            System.out.println("\n[" + new Date() + "]");
            System.out.println("Received request from " + ip + ":" + port + " for " + uri + " {");
            while (line != null) {
                if (!line.isEmpty()) {
                    System.out.println("     " + line);
                } else {
                    break;
                }
                line = bufferedReader.readLine();
            }
            parseSuccess = true;
        } catch (IOException e) {
            e.printStackTrace();
        }
        System.out.println("}");
    }

    public boolean isParseSuccess() {
        return parseSuccess;
    }

    public String getMethod() {
        return method;
    }

    public String getUri() {
        return uri;
    }

    public String getHttpVersion() {
        return httpVersion;
    }
}
