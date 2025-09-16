package vn.neosoft;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

@RestController
public class AirController {
    function function = new function();
    @GetMapping("/write_log")
    public String ghilog(String message)
    {
    if(!message.isEmpty() || message != null) {
        function.log(message);
        return "200";
    }
    else
        return "400";
    }
    @GetMapping("/read_log")
    public String doclog()
    {
        String data = function.readLog();
        if(!data.equals("400"))
        {
            return data;
        }
        else
            return "400";

    }


}
