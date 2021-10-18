package net.yxchen.lab1;

import java.io.*;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.Properties;

public class ServerUtils {
    private static String localIp;
    private static int port;
    private static String basePath;  // web 服务器基础路径

    public static boolean load() {
        try {
            InputStream in = new BufferedInputStream(new FileInputStream("config.properties"));
            Properties prop = new Properties();
            prop.load(new InputStreamReader(in, StandardCharsets.UTF_8));  // 中文支持
            localIp = prop.getProperty("LocalIp");
            port = Integer.parseInt(prop.getProperty("Port"));
            basePath = prop.getProperty("BasePath");
            File file = new File(basePath);
            if (!file.exists()) {
                System.err.println("base " + basePath + " does not exist!");
            } else if (!file.isDirectory()) {
                System.err.println("base " + basePath + " is not a directory!");
            } else {
                return true;
            }
        } catch (FileNotFoundException e) {
            System.out.println("config.properties does not exist!");
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return false;
    }

    public static String getLocalIp() {
        return localIp;
    }

    public static int getPort() {
        return port;
    }

    public static String getBasePath() {
        return basePath;
    }
}
