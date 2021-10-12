package net.yxchen.lab1;

import java.io.*;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Properties;

public class HttpServer {
    private static ServerSocket serverSocket = null;

    public static void main(String[] args) {
        // 加载配置
        if (!ServerUtils.load()) {
            return;
        }
        System.out.println("Start HttpServer, located on " + ServerUtils.getLocalIp()
                + ":" + ServerUtils.getPort() + ", base path is " + ServerUtils.getBasePath());

        // 注册 ctrl-c 结束程序
        Runtime.getRuntime().addShutdownHook(new Thread(() -> {
            System.out.println("Shutdown...");
            if (serverSocket != null && !serverSocket.isClosed()) {
                try {
                    serverSocket.close();
                    System.out.println("Server closed");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }));
        System.out.println("Press ctrl+c to shutdown");

        // 分发请求
        try {
            serverSocket = new ServerSocket(ServerUtils.getPort(), 50,
                    InetAddress.getByName(ServerUtils.getLocalIp()));
            System.out.println("Listening on " + ServerUtils.getLocalIp() + ":" + ServerUtils.getPort());

            while (true) {
                Socket socket = serverSocket.accept();
                new Thread(new Receiver(socket)).start();
            }
        } catch (Exception e) {
            e.printStackTrace();
            return;
        }
    }
}
