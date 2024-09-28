#include "Timeline.h"
#include "GameEngine.h"

int64_t Timeline::global_start_time = Timeline::now();

Timeline::Timeline(Timeline* anchor, int64_t tic, TimelineType type) {
    this->anchor = anchor;
    this->tic = tic;
    this->type = type;
    this->start_time = (type == TimelineType::Global) ? global_start_time : now();
    this->elapsed_pause_time = 0;
    this->last_paused_time = 0;
    this->paused = false;
    this->speed = 1.0;
    this->last_time = 0;
    this->last_real_time = start_time;
}

Timeline::Timeline() {
    this->anchor = nullptr;
    this->tic = 1;
    this->type = TimelineType::Global;
    this->start_time = now();
    this->elapsed_pause_time = 0;
    this->last_paused_time = 0;
    this->paused = false;
    this->speed = 1.0;
    this->last_time = 0;
    this->last_real_time = start_time;
}

// Initialize the timeline, create a global timeline if the type is global else create a local timeline too
bool Timeline::initialize(TimelineType type) {
	// used for the server
	if (type == TimelineType::Global) {
		globalTimeline = new Timeline();
		return true;
	}
    else {
		//used for clients or single player
		globalTimeline = new Timeline();
		createNewLocalTimeline(); // create the initial local timeline
		return true;
    }
}
Timeline::~Timeline() {
	delete globalTimeline;
	delete currentLocalTimeline;
}

int64_t Timeline::now() {
    return std::chrono::high_resolution_clock::now().time_since_epoch().count();
}

int64_t Timeline::getTime() {
    std::lock_guard<std::mutex> lock(m);

    // if global timeline, return the current time
    int64_t current_real_time = (anchor != nullptr) ? anchor->getTime() : now();

    if (paused) {
        return last_time;
    }

    int64_t real_elapsed = (current_real_time - last_real_time) / tic;
    int64_t scaled_elapsed = static_cast<int64_t>(real_elapsed * speed);
    last_time += scaled_elapsed;
    last_real_time = current_real_time;

    return last_time;
}

void Timeline::pause() {
    std::lock_guard<std::mutex> lock(m);
    if (!paused) {
        getTime(); // Update last_time before pausing
        last_paused_time = (anchor != nullptr) ? anchor->getTime() : now();
        paused = true;
    }
}


void Timeline::resume() {
    std::lock_guard<std::mutex> lock(m);
    if (paused) {
        int64_t current_time = (anchor != nullptr) ? anchor->getTime() : now();
        elapsed_pause_time += current_time - last_paused_time;
        last_real_time = current_time;
        paused = false;
    }
}

void Timeline::reset() {
    std::lock_guard<std::mutex> lock(m);
    start_time = (type == TimelineType::Global) ? global_start_time :
        ((anchor != nullptr) ? anchor->getTime() : now());
    elapsed_pause_time = 0;
    last_paused_time = 0;
    last_time = 0;
    last_real_time = start_time;
    paused = false;
}

void Timeline::setAnchor(Timeline* new_anchor) {
    std::lock_guard<std::mutex> lock(m);
    if (anchor != new_anchor) {
        int64_t current_time = getTime();
        anchor = new_anchor;
        reset();
        last_time = current_time;
    }
}

// sets the tic of the timeline
void Timeline::changeTic(int64_t new_tic) {
    std::lock_guard<std::mutex> lock(m);
    if (tic != new_tic) {
        getTime(); // Update last_time before changing tic
        tic = new_tic;
    }
}

bool Timeline::isPaused() const {
    return paused;
}

void Timeline::setSpeed(double new_speed) {
    std::lock_guard<std::mutex> lock(m);
    if (speed != new_speed) {
        getTime(); // Update last_time before changing speed
        speed = new_speed;
    }
}

double Timeline::getSpeed() {
    std::lock_guard<std::mutex> lock(m);
    return speed;
}

void Timeline::createNewLocalTimeline() {
    std::lock_guard<std::mutex> lock(m); 
    if (currentLocalTimeline) {
        delete currentLocalTimeline; // clean up old timeline 
    }
    currentLocalTimeline = new Timeline(globalTimeline, 1);
}
