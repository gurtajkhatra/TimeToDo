//init function
//Deinit function
//Update layer function (draw timer)
//Apptimercallback 

#include <pebble.h>
#include <math.h>
#include <string.h>
  
//Constants of pebble device
#define SCREEN_HEIGHT 168
#define SCREEN_WIDTH 144  

//Constants for Bar animation
#define TIMER_BAR_INITAL_WIDTH (SCREEN_WIDTH-20)
#define TIMER_BAR_X 10
#define TIMER_BAR_Y 72
#define TIMER_BAR_HEIGHT 10
#define NUMBER_OF_UPDATES TIMER_BAR_INITAL_WIDTH
  
//Constants for Timer Numbers
#define TIMER_NUM_X 0
#define TIMER_NUM_Y (TIMER_BAR_Y - 65)
#define TIMER_NUM_WIDTH (SCREEN_WIDTH)
#define TIMER_NUM_HEIGHT (TIMER_BAR_Y - TIMER_NUM_Y - 10)
  
//Constant for Task Name
#define TASK_X 10
#define TASK_Y (TIMER_BAR_Y + 20)
#define TASK_WIDTH (SCREEN_WIDTH-20)
#define TASK_HEIGHT (SCREEN_HEIGHT-TASK_Y)
  
//Constants for Tasks and Time
#define MAX_NUMBER_OF_TASKS 20
#define MAX_TASK_STRING_SIZE 21

//COMMUNICATION AND DATA VARIABLES
char *listOfTasks[MAX_NUMBER_OF_TASKS];
int listOfTimes[MAX_NUMBER_OF_TASKS];


//DESIGN OF GRAPHICS AND TEXT VARIABLES:

//Window 
Window *mainWindow;

//Variables used to draw the timer animation
Layer *barLayer;
AppTimer *barTimer;
int barCurrentWidth;
GRect timerBar;
double percentDone = 1; 
int clockRunning = 0;

//Variables to draw timer numbers:
TextLayer *timerLayer;
AppTimer *timerTimer;
int initalTime;
int timeLeft;
char timeLeftString[8];
int minutesLeft;
int secondsLeft;

//Variables to write task
TextLayer *taskLayer;
char *taskName;
int currentTask=0;
int initialized;

//Variables to swtich between tasks
int totalTasks;

  
//FUNCTIONS FOR COMMUNICATION WITH WEBSITE/DATA TRASNFER

//Goes through the list of values passed to it, and puts them in the proper array
void process_tuple(Tuple *t){
  int key = t->key;
  //If the key is even, its a string, and if the key is odd its the time in seconds
  if(key%2 == 0){ 
    listOfTasks[key/2] = t->value->cstring;
    totalTasks++;
  }
  else if(key%2 != 0){
    printf("Value: %d", t->value->uint16);
    listOfTimes[key/2] = t->value->uint16;
  }
}
 
void inbox(DictionaryIterator *iter, void *context){
  Tuple *t = dict_read_first(iter);
  if(t){
    process_tuple(t);
  }
  while(t != NULL){
    t = dict_read_next(iter);
    if(t){
      process_tuple(t);
    }
  }
  initialized = 0;
}

//Sets the variables for the name of the task and time for it, 
//starting from Task #0
void setTaskAndTime(int taskNumber){
  //printf("IN SET TASK AND TIME TASK NAME: %s TASK TIME: %d", listOfTasks[0], listOfTimes[0]);
  taskName = listOfTasks[currentTask];
  initalTime = listOfTimes[currentTask];
  timeLeft = initalTime;
  //If the data hasn't been synced to the program yet, it isn't fully initalized yet
  if (taskName == NULL) {
    initialized = 0;
  }
}

//FUNCTIONS FOR DRAWING ON PEBBLE SCREEN

//Update procedure for the time left bar
void barUpdater(Layer *layer, GContext *ctx){
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(TIMER_BAR_X, TIMER_BAR_Y, barCurrentWidth, TIMER_BAR_HEIGHT), 0, GCornerNone);
  graphics_draw_rect(ctx, GRect(TIMER_BAR_X, TIMER_BAR_Y, TIMER_BAR_INITAL_WIDTH, TIMER_BAR_HEIGHT));
}

//Update the value displayed on the timer
void timerNumberUpdater(){
  minutesLeft = timeLeft/60;
  secondsLeft = timeLeft%60;
  if (minutesLeft<10 && secondsLeft<10){
    snprintf(timeLeftString, 15, "0%d:0%d", minutesLeft, secondsLeft);
  }
  else if(minutesLeft<10){
    snprintf(timeLeftString, 15, "0%d:%d", minutesLeft, secondsLeft);
  }
  else if(secondsLeft<10){
    snprintf(timeLeftString, 15, "%d:0%d", minutesLeft, secondsLeft);
  }
  else{
    snprintf(timeLeftString, 15, "%d:%d", minutesLeft, secondsLeft);
  }
  text_layer_set_text(timerLayer, timeLeftString);
}

//Sets the time and task layers to properly display
void setTimeAndTaskLayers(){
  text_layer_set_text(taskLayer, taskName);
  timerNumberUpdater();
}

//AppTimer that runs every second
void timerCallback(){
  if(!initialized){
    initialized = 1;
    barCurrentWidth = TIMER_BAR_INITAL_WIDTH;
    setTaskAndTime(currentTask);
    setTimeAndTaskLayers();
    timerTimer = app_timer_register(100, timerCallback, NULL);
    
  }
  else if(!clockRunning){
    //setTimeAndTaskLayers();
    timerTimer = app_timer_register(1000, timerCallback, NULL);
  }
  else if(timeLeft && clockRunning){
    timeLeft -= 1;
    timerNumberUpdater();
    timerTimer = app_timer_register(1000, timerCallback, NULL);
    barCurrentWidth = (int)(TIMER_BAR_INITAL_WIDTH*percentDone);
    percentDone = (double)timeLeft/initalTime;
    layer_mark_dirty(barLayer);
  }
  else {
    barCurrentWidth = 0;
    layer_mark_dirty(barLayer);
    vibes_long_pulse();
    //If currently on the last task on the list
    if(currentTask == (totalTasks-1)) {
    //set the current task to the first one
      currentTask = 0;
    }
    else {
      currentTask++;
    }
    clockRunning = 0;
    initialized = 0;
    timerTimer = app_timer_register(1000, timerCallback, NULL);
  }
}



//METHODS TO SETUP BUTTONS

//What to do when the middle button is clicked once
void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(clockRunning){
    clockRunning = 0;
  }
  else{
    clockRunning = 1;
  }
}


//What to do when the up button is clicked once
void up_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(currentTask == 0) {
    currentTask = totalTasks-1;
  }
  else{
    currentTask--;
  }
  initialized = 0;
}

//What to do when the down button is clicked once
void down_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  //If currently on the last task on the list
  if(currentTask == (totalTasks-1)) {
    //set the current task to the first one
    currentTask = 0;
    
  }
  else {
    currentTask++;
  }
  initialized = 0;
}

//Declare the buttons your would like to subscribe to
void config_provider(Window *window) {
 // single click on middle button
  window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
  //single click on up button
  window_single_click_subscribe(BUTTON_ID_UP, up_single_click_handler);
  //single click on down button
  window_single_click_subscribe(BUTTON_ID_DOWN, down_single_click_handler);
}



void init(){
  totalTasks = 0;  
  //Set up communications with Website
   app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  app_message_register_inbox_received(inbox);
  //app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
 

  //Initialize Window and Layers
  mainWindow = window_create();
  window_set_background_color(mainWindow, GColorWhite);
  Layer *root = window_get_root_layer(mainWindow);
  //Layer for bar animation
  barLayer = layer_create(GRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
  layer_set_update_proc(barLayer, barUpdater);
  layer_add_child(root, barLayer);
  
  //Layer for timer number
  timerLayer = text_layer_create(GRect(TIMER_NUM_X, TIMER_NUM_Y, TIMER_NUM_WIDTH, TIMER_NUM_HEIGHT));
  text_layer_set_font(timerLayer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_color(timerLayer, GColorBlack);
  text_layer_set_text_alignment(timerLayer, GTextAlignmentCenter);
  layer_add_child(root, text_layer_get_layer(timerLayer));
  timeLeft = listOfTimes[0];
  minutesLeft = timeLeft/60;
  secondsLeft = timeLeft%60;
  
  //Layer for task name
  taskLayer = text_layer_create(GRect(TASK_X, TASK_Y, TASK_WIDTH, TASK_HEIGHT));
  text_layer_set_font(taskLayer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(taskLayer, GTextAlignmentCenter);
  window_stack_push(mainWindow, true);
  layer_add_child(root, text_layer_get_layer(taskLayer));
  
  //set up Buttons config provider
  window_set_click_config_provider(mainWindow, (ClickConfigProvider) config_provider);

  //Initalize other variables
  barCurrentWidth = TIMER_BAR_INITAL_WIDTH; 
  
  //Initialize timers (make sure this is at the end, anything called after this in init will not be
  //called)
  initialized = 0;
  timerTimer = app_timer_register(100, timerCallback, NULL);
  //barTimer = app_timer_register((int)(secondsPerUpdate*1000), barTimerCallback, NULL);
}

void deinit(){
  layer_destroy(barLayer);
  text_layer_destroy(timerLayer);
  text_layer_destroy(taskLayer);
  window_destroy(mainWindow);
  app_timer_cancel(barTimer);
  app_timer_cancel(timerTimer);
}


  
int main(){
  init();
  app_event_loop();
  deinit();
}