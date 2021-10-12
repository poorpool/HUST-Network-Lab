package net.yxchen.lab1;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public class Receiver implements Runnable{
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

            Request request = new Request(inputStream);
            request.parse(socket.getInetAddress().getHostAddress(), socket.getPort());
            Response response = new Response(outputStream);
            response.constructResponse(request);
            response.fillResponse();
        } catch (IOException e) {
            e.printStackTrace();
            return;
        }
        socketClose();
    }
}
