package vn.neosoft;

import java.io.*;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.regex.Pattern;


public class function{
    String duongdanlog="/var/lib/tomcat10/webapps/log_air_system.txt";

    public void log(String messagee) {
        String timeStamp = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date());
        try (PrintWriter writer = new PrintWriter(new FileWriter(duongdanlog, true))) {
            writer.print(timeStamp+" - ");
            writer.println(messagee);
        } catch (Exception ex) {
            //ex.printStackTrace();
        }}
    public String readLog() {
        StringBuilder logContent = new StringBuilder(); // StringBuilder để chứa toàn bộ nội dung file log
        try (BufferedReader reader = new BufferedReader(new FileReader(duongdanlog))) {
            String line;
            while ((line = reader.readLine()) != null) {
                logContent.append(line).append("\n"); // Thêm từng dòng vào StringBuilder, kèm ký tự xuống dòng
            }
        } catch (Exception ex) {
            return "400"; // Trả về mã lỗi nếu có exception
        }
        return logContent.toString(); // Trả về toàn bộ nội dung file log
    }



}
