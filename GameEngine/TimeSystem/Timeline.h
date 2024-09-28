#pragma once
#include <mutex>
#include <chrono>

enum class TimelineType {
    Local,
    Global
};

class Timeline {
private:
    std::mutex m;                   // for thread safety
    int64_t elapsed_pause_time;      
    int64_t last_paused_time; 
	int64_t start_time;              // Start time of the timeline
    int64_t tic;                     // Time units relative to anchor
    bool paused;                     
    Timeline* anchor;                
    TimelineType type;               // Type of timeline (Local or Global)
    static int64_t now();            // gets current time
	static int64_t global_start_time; //start time of the global timeline
	double speed;                    // for time manipulation
    int64_t last_time;               // lst calculated logical time
    int64_t last_real_time;          // al time used for calculation

    Timeline* globalTimeline;
    Timeline* currentLocalTimeline;


public:
    // Constructor for client timelines with anchor
    Timeline(Timeline* anchor, int64_t tic, TimelineType type = TimelineType::Global);

	// for the server's global timeline, assuming global timelines dont require an anchor
	Timeline();

    bool initialize(TimelineType type);

    ~Timeline();

    // Get elapsed time in ns
    int64_t getTime();

    void pause();

    void resume();

    void reset();

    //  set new anchor
    void setAnchor(Timeline* anchor);

    // Change the tic value
    void changeTic(int64_t new_tic);
    
    int64_t getTic();

    // Check if timeline is paused
    bool isPaused() const;

    // Set the speed of time progression
    void setSpeed(double new_speed);

    // Get the current speed of time progression
    double getSpeed() const;

    void createNewLocalTimeline();
};