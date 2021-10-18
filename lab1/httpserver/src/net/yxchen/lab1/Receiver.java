package net.yxchen.lab1;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class Receiver implements Runnable{
    // 收到的待处理的 socket
    private Socket socket;

    public Receiver(Socket socket) {
        this.socket = socket;
    }

    private void socketClose() {
        if (socket != null && !socket.isClosed()) {
            try {
                socket.close();
                System.out.println("socket closed");
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }
    @Override
    public void run() {
        try {
            InputStream inputStream = socket.getInputStream();
            OutputStream outputStream = socket.getOutputStream();

            // 根据 socket 构造一个 request
            Request request = new Request(inputStream);
            request.parse(socket.getInetAddress().getHostAddress(), socket.getPort());

            // 根据 request 指定响应
            Response response = new Response(outputStream);
            response.constructResponse(request);

            // 写入响应
            response.fillResponse();
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
        socketClose();
    }
}
