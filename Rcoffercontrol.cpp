

    
#include <unordered_set>
#include <mutex>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <errno.h>
#include <vector>   //for using vectors instead of arrays
#include <map>
#include <array>
 #include <signal.h>
 #include "crow_all"
#include <chrono>
#include <iostream>
#include <thread>
 #include <unordered_set>
#include <JetsonGPIO.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;
class http_server
{
private:
    crow::SimpleApp app;
    std::mutex mtx;;
    std::unordered_set<crow::websocket::connection*> users;
    thread *t;

public:
    http_server();
  //  http_server();
    ~http_server(){};
    void start_server();
    void send_message(string text);

    // variable
    bool isSettingChanged;
    bool isConnected;

    string robot_cmd;
    string data;
    
};

http_server::http_server()
{
    this->isConnected = false;
    this->isSettingChanged = true;
         
    this->robot_cmd = "";
    this->data = "";

   std::cout<<"hellow"<<std::endl;
       t = new thread(&http_server::start_server,this);
}



void http_server::start_server()
{

    CROW_ROUTE(this->app, "/ws")
        .websocket()
        .onopen([&](crow::websocket::connection& conn){
                    std::cout<<"hellow"<<std::endl;
                CROW_LOG_INFO << "new websocket connection";
                std::lock_guard<std::mutex> _(mtx);
                users.insert(&conn);
     
                this->isConnected = true;



                })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason){
                CROW_LOG_INFO << "websocket connection closed: " << reason;
                std::lock_guard<std::mutex> _(mtx);
                users.erase(&conn);
                })

        .onmessage([&](crow::websocket::connection& conn, const std::string& data, bool is_binary){
                std::lock_guard<std::mutex> _(mtx);
                std::cout<<"data77777"<<std::endl;
                this->data = "6";
               // this->data =data;  
                if ((data.find("Read_button")!=string::npos))
                  {
         
                    std::cout<<data<<std::endl;
  
                     this->data = data;
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                       this->data = "";
                  }else 
                  {
                   this->data = "6";
                  }
 
               
                });

    CROW_ROUTE(app, "/")
    ([]{
        char name[256];
        gethostname(name, 256);
        crow::mustache::context x;
        x["servername"] = name;
	
        auto page = crow::mustache::load("setting.html");
        
        
        return page.render(x);
     });

     

     

    app.port(45680)
        .multithreaded()
        .run();
}


static bool end_this_program = false;

inline void delay(int s) { this_thread::sleep_for(chrono::seconds(s)); }



int main()
{
    http_server server;
    // When CTRL+C pressed, signalHandler will be called
   

      int led_pin = 12; // BOARD pin 12
//     int but_pin = 18; // BOARD pin 18
  GPIO::setmode(GPIO::BOARD);
   GPIO::setup(led_pin, GPIO::OUT); 
    GPIO::output(led_pin, GPIO::LOW);



    cout << "Starting demo now! Press CTRL+C to exit" << endl;
    
    while (true)
    {
              //  auto start_time = std::chrono::steady_clock::now();
        if (server.isSettingChanged){
            server.isSettingChanged = false;
             std::cout<<"Hi2222"<<std::endl;
        //    UpdateMotorSetting(&arm_joints, &motor_setting);
        }
         std::cout<<server.data<< "Check"<<std::endl;
          //server.data = "";
    //stoi(server.data);
    int button;
    string  data2;
    string abs;
   if (server.data  ==  ""){
   abs ="6";
   }
   else
   {
    abs = server.data; 
    json b= json::parse(abs);
     data2 = b[ "Read_button"];
     button = stoi(data2);
    
   }
   std::cout<< abs << std::endl;
    std::cout<<data2 << std::endl;
   cout << "Button Pressed!" << endl;
       switch(button)
      { 
      case 1 :
    std::cout<<"button_on_1"<<std::endl;  
    GPIO::output(led_pin, GPIO::HIGH);
       delay(3);
   GPIO::output(led_pin, GPIO::LOW);
      break;
  
}}}

