#include "panel_positioning.h"

Direction dir = RIGHT, orientation = TOP;
// Direction dir = NONE, orientation = NONE;
volatile uint8_t panelCount = 0;
bool isDetect = false;

static uint8_t oldCount;
volatile bool isPauseSearch = false;
volatile bool isSearch = false;

// state changing functions for used in external interrupt
void panelCountInc(void) {++panelCount;}
void resumeSearch(void) {isPauseSearch = false;}

void startSearch(void);

// initiate the detection and search procedure
void startDetectPanel(void) {
    isDetect = true;
    isSearch = false;
    dir = NONE;

    signal_detect_rising_gpio_init(&ALL_DIR_PORT);

    // HAL_I2C_MspDeInit(&hi2c1);
    signal_emit_gpio_init(&SDA_PORT);
    HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_SET);

    signal_detect_falling_gpio_init(&SCL_PORT);

    while(!isSearch); // wait until it detects an incoming signal

    HAL_GPIO_DeInit(SCL_GPIO_Port, SCL_Pin);
    signal_emit_gpio_init(&SCL_PORT);
    HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(SCL_GPIO_Port, SCL_Pin, GPIO_PIN_SET);

    startSearch();
}

void stopDetectPanel(void) {isDetect = false;}

void startSearch_master(void) {
    isDetect = false;
    isSearch = true;
    dir = RIGHT; // debug
    orientation = TOP; // debug

    signal_detect_falling_gpio_init(&SDA_PORT);
    signal_detect_falling_gpio_init(&SCL_PORT);

    HAL_Delay(25);
    startSearch();
}

// a look up table for determining which direction of the panel is actually the top direction
const Direction dir_look_up[4][4] = {
    {BOTTOM, TOP   , LEFT  , RIGHT },
    {RIGHT , LEFT  , BOTTOM, TOP   },
    {TOP   , BOTTOM, RIGHT , LEFT  },
    {LEFT  , RIGHT , TOP   , BOTTOM}
};

__forceinline uint32_t normalizeTime(uint32_t t1, uint32_t t2) {
    return t2 - t1;
}

static void delay(uint32_t time) {
    uint32_t endTime = HAL_GetTick() + time;
    while (endTime - HAL_GetTick() > 0);
}

void detectPanel(Direction d) {
    if (!isDetect) return;

    // HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

    // Circular buffer for rising edge time
    static uint32_t timeBuf[4][3];
    static uint8_t head[4] = {0};

    timeBuf[d][head[d]] = HAL_GetTick();
    head[d] = ++head[d] % 3;

    // get the time difference between three rising edge
    uint32_t t1 = normalizeTime(timeBuf[d][head[d]], timeBuf[d][(head[d]+1)%3]);
    uint32_t t2 = normalizeTime(timeBuf[d][(head[d]+1)%3], timeBuf[d][(head[d]+2)%3]);

    // determine the direction if it is a valid signal
    if (t1 == short_time) {
        if (t2 == short_time) orientation = dir_look_up[d][LEFT];
        else if (t2 == long_time) orientation = dir_look_up[d][RIGHT];
    } else if (t1 == long_time) {
        if (t2 == short_time) orientation = dir_look_up[d][TOP];
        else if (t2 == long_time) orientation = dir_look_up[d][BOTTOM];
    }
    if (orientation != NONE) dir = d;

    // if detected a panel at a specific direction, initiate the panel searching procedure on other three direction
    if (dir != NONE) {
        stopDetectPanel();
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        isSearch = true;
    }
}

// Function to emit signal
__forceinline static void setHigh(const GPIO* gpio, uint32_t delay_time) {
    gpio->port->BSRR = gpio->pin;
    delay(1);
    gpio->port->BRR = gpio->pin;
    delay(delay_time - 1);
}

__forceinline static void emit_top_signal(const GPIO* gpio) { // long - short
    setHigh(gpio, long_time);
    setHigh(gpio, short_time);
    setHigh(gpio, stop_time);
}

__forceinline static void emit_bottom_signal(const GPIO* gpio) { // long - long
    setHigh(gpio, long_time);
    setHigh(gpio, long_time);
    setHigh(gpio, stop_time);
}

__forceinline static void emit_left_signal(const GPIO* gpio) { // short - short
    setHigh(gpio, short_time);
    setHigh(gpio, short_time);
    setHigh(gpio, stop_time);
}

__forceinline static void emit_right_signal(const GPIO* gpio) { // short - long
    setHigh(gpio, short_time);
    setHigh(gpio, long_time);
    setHigh(gpio, stop_time);
}

void emitSignal(Direction d, const GPIO* gpio) {
    // determine the actural direction signal need to be sent
    Direction dirSignal = d;
    switch (orientation) {
        case LEFT:   dirSignal = (Direction) ((d + 3) % 4); break;
        case BOTTOM: dirSignal = (Direction) ((d + 2) % 4); break;
        case RIGHT:  dirSignal = (Direction) ((d + 1) % 4); break;
        default: break;
    }

    for (uint8_t i = 0; i < emit_times; ++i) {
        if (panelCount != oldCount){ // if panelCount is different from oldCount, it means there is a new panel detected, pause the search and wait
            isPauseSearch = true;
            signal_detect_falling_gpio_init(gpio);
            while(isPauseSearch);
            break; // panel is detected in this side, no need to emit signal anymore
        }
        switch (dirSignal) {
            case TOP:    emit_top_signal(gpio); break;
            case LEFT:   emit_left_signal(gpio); break;
            case BOTTOM: emit_bottom_signal(gpio); break;
            case RIGHT:  emit_right_signal(gpio); break;
        }
    }
    
    HAL_GPIO_DeInit(gpio->port, gpio->pin); // deinit the port to prevent anymore interrupt to mess up the search
}

void startSearch(void) {
    // switch all signal pin to output
    signal_emit_gpio_init(&ALL_DIR_PORT);

    // set the pin of the signal coming direction high to indicate the search procedure is not done
    switch (dir) {
        case TOP:    HAL_GPIO_WritePin(TOP_GPIO_Port, TOP_Pin, GPIO_PIN_SET); break;
        case LEFT:   HAL_GPIO_WritePin(LEFT_GPIO_Port, LEFT_Pin, GPIO_PIN_SET); break;
        case BOTTOM: HAL_GPIO_WritePin(BOTTOM_GPIO_Port, BOTTOM_Pin, GPIO_PIN_SET); break;
        case RIGHT:  HAL_GPIO_WritePin(RIGHT_GPIO_Port, RIGHT_Pin, GPIO_PIN_SET); break;
    }

    // switch SCL to falling edge interrupt pin to init I2C when there is no panel being undiscovered
    HAL_GPIO_WritePin(SDA_GPIO_Port, SDA_Pin, GPIO_PIN_RESET);
    signal_detect_falling_gpio_init(&SDA_PORT);

    isPauseSearch = false;
    
    const GPIO* gpio_set[4] = {&TOP_PORT, &LEFT_PORT, &BOTTOM_PORT, &RIGHT_PORT};
    for (Direction i = TOP; i < 4; ++i) {
        if (!isSearch) break; // nothing else to search, then terminate the search procedure
        oldCount = panelCount; // remember the original panel count
        if (dir != i) emitSignal(i, gpio_set[i]);
    }

    // set the pin of the signal coming direction low to indicate the searching procedure is not done
    switch (dir) {
        case TOP:    HAL_GPIO_WritePin(TOP_GPIO_Port, TOP_Pin, GPIO_PIN_RESET); break;
        case LEFT:   HAL_GPIO_WritePin(LEFT_GPIO_Port, LEFT_Pin, GPIO_PIN_RESET); break;
        case BOTTOM: HAL_GPIO_WritePin(BOTTOM_GPIO_Port, BOTTOM_Pin, GPIO_PIN_RESET); break;
        case RIGHT:  HAL_GPIO_WritePin(RIGHT_GPIO_Port, RIGHT_Pin, GPIO_PIN_RESET); break;
    }
}
