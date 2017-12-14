#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <cassert>
#include <array>

class ElevatorController
{
public:
    // No getters to save time
    int floors;
    int height; // meters
    int speed; // m/s
    int door_time; // sec

    ElevatorController()
    {
        stop_thread.store(false);
        floors = 0;
        height = 0;
        speed = 0;
        door_time = 0;

        cur_speed = 0;
        elevator_height = 0;
        destination_floor = 0;
        door_closed = true;
        door_opened_at = 0;

        for (int i = 0; i < 20; i++)
        {
            pressed[i] = false;
        }
    }

    void call(int floor)
    {
        std::lock_guard<std::mutex> lock(m);
        pressed[floor - 1] = true;
    }

    void stop() { stop_thread.store(true); }
    static void process_s(ElevatorController* data);
    void process();

private:
    std::atomic<bool> stop_thread;

    std::mutex m;
    std::array<bool, 20> pressed;

    int cur_speed;
    int elevator_height;
    int destination_floor;
    bool door_closed;
    int door_opened_at;
};


void ElevatorController::process_s(ElevatorController* data)
{
    data->process();
}

void ElevatorController::process()
{
    using namespace std::chrono_literals;
    std::cout << "Elevator stopped on floor " << (elevator_height / height + 1) << "\n";
    auto world_start = std::chrono::system_clock::now();
    auto start = world_start;
    while(!stop_thread)
    {
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> start_time = start - world_start; // sec
        std::chrono::duration<double> end_time = end - world_start; // sec

        int s = ceil(start_time.count());
        int e = floor(end_time.count());

        // process each second
        for (int t = s; t <= e; t++)
        {
            //std::cout << "Tick " << t << "\n";
            if (cur_speed == 0)
            {
                if (door_closed)
                {
                    std::lock_guard<std::mutex> lock(m);
                    int f = elevator_height / height;
                    for (int i = f - 1; i >= 0; i--)
                    {
                        if (pressed[i])
                        {
                            std::stringstream msg;
                            msg << "Elevator called to floor " << (i + 1) << "\n";
                            std::cout << msg.str();
                            cur_speed = -speed;
                            break;
                        }
                    }
                    if (cur_speed == 0)
                    {
                        for (int i = f + 1; i < floors; i++)
                        {
                            if (pressed[i])
                            {
                                std::stringstream msg;
                                msg << "Elevator called to floor " << (i + 1) << "\n";
                                std::cout << msg.str();
                                cur_speed = +speed;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if ((t - door_opened_at) == door_time)
                    {
                        int f = elevator_height / height;
                        std::stringstream msg;
                        msg << "Closing elevator door on floor " << (f + 1) << "\n";
                        std::cout << msg.str();

                        door_closed = true;
                    }
                }
            }
            else
            {
                assert(door_closed);
                int p = elevator_height;
                int pf = elevator_height / height +
                    ((cur_speed > 0) - (cur_speed < 0));
                elevator_height += cur_speed;
                int mf = elevator_height / height;
                if (cur_speed > 0)
                {
                    for (int f = pf; f <= mf; f++)
                    {
                        std::lock_guard<std::mutex> lock(m);
                        if (pressed[f])
                        {
                            std::stringstream msg;
                            msg << "Open elevator door on floor " << (f + 1) << "\n";
                            std::cout << msg.str();

                            door_closed = false;
                            cur_speed = 0;
                            pressed[f] = false;
                            door_opened_at = t;
                            elevator_height = f * height;
                            break;
                        }
                        else if ((cur_speed < 0) && (f == 0) ||
                            (cur_speed > 0) && (f == floors))
                        {
                            assert(false);
                        }
                        else
                        {
                            std::stringstream msg;
                            msg << "Elevator passed the floor " << (f + 1) << "\n";
                            std::cout << msg.str();
                        }
                    }
                }
                else
                {
                    for (int f = pf; f >= mf; f--)
                    {
                        std::lock_guard<std::mutex> lock(m);
                        if (pressed[f])
                        {
                            std::stringstream msg;
                            msg << "Open elevator door on floor " << (f + 1) << "\n";
                            std::cout << msg.str();

                            door_closed = false;
                            cur_speed = 0;
                            pressed[f] = false;
                            door_opened_at = t;
                            elevator_height = f * height;
                            break;
                        }
                        else if ((cur_speed < 0) && (f == 0) ||
                            (cur_speed > 0) && (f == floors))
                        {
                            assert(false);
                        }
                        else
                        {
                            std::stringstream msg;
                            msg << "Elevator passed the floor " << (f + 1) << "\n";
                            std::cout << msg.str();
                        }
                    }                    
                }
            }
            start += std::chrono::duration<int>(1);
        }

        std::this_thread::sleep_for(0ms);
    }
}

int main(int argc, char *argv[])
{
    using namespace std::chrono_literals;
    if (argc != 5)
    {
        std::cout << "Wrong number of arguments\n";
        return 1;
    }
    ElevatorController data;
    try
    {
        data.floors = std::stoi( argv[1] );
        data.height = std::stoi( argv[2] );
        data.speed = std::stoi( argv[3] );
        data.door_time = std::stoi( argv[4] );
    }
    catch (std::invalid_argument)
    {
        std::cout << "Wrong argument\n";
        return 2;
    }
    if (data.floors < 5 || data.floors > 20)
    {
        std::cout << "Wrong number of floors\n";
        return 3;
    }
    std::cout << "Number of floors: " << data.floors << "\n";
    if (data.height <= 0)
    {
        std::cout << "Wrong height\n";
        return 4;
    }
    std::cout << "Elevator height: " << data.height << " m\n";
    if (data.speed <= 0)
    {
        std::cout << "Wrong speed\n";
        return 5;
    }
    std::cout << "Elevator speed: " << data.speed << " m/s\n";
    if (data.door_time < 0)
    {
        std::cout << "Wrong time\n";
        return 6;
    }
    std::cout << "Opened door time: " << data.door_time << " s\n";
    std::cout << "\n";

    std::thread process_thread(ElevatorController::process_s, &data);
    do
    {
        std::stringstream msg;
        msg << "Choose:\n"
          << "[1] Call elevator and then enter floor\n"
          << "[2] Press button and then enter floor where it pressed\n"
          << "[3] Exit\n";
        std::cout << msg.str();
        std::string input;
        std::getline(std::cin, input);

        if (input == "1") {
            std::getline(std::cin, input);
            try
            {
                int n = std::stoi( input );
                if (n < 1 || n > data.floors)
                {
                    std::cout << "Wrong floor\n";
                    continue;
                }
                std::stringstream msg2;
                msg2 << "Call elevator from floor " << n << "\n";
                std::cout << msg2.str();
                data.call(n);
            }
            catch (std::invalid_argument)
            {
            }
        }
        else if (input == "2") {
            std::getline(std::cin, input);
            try
            {
                int n = std::stoi( input );
                if (n < 1 || n > data.floors)
                {
                    std::cout << "Wrong floor\n";
                    continue;
                }
                std::stringstream msg2;
                msg2 << "Press floor button " << n << "\n";
                std::cout << msg2.str();
                data.call(n);
            }
            catch (std::invalid_argument)
            {
            }
        }
        else if (input == "3") {
            data.stop();
            break;
        }
        else {
            std::cout << "Invalid!\n\n";
        }
    } while(true);

    process_thread.join();
}