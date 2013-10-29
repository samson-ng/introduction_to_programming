namespace octet {

  class block {

    mat4t modelToWorld;
    float halfWidth;
    float halfHeight;
    vec4 color;


  public:

    int x;
    int y;

    block() {
    }

    void init(const vec4 &_color, float x, float y, float w, float h) {
      modelToWorld.loadIdentity();
      modelToWorld.translate(x, y, 0);
      halfWidth = w * 0.5f;
      halfHeight = h * 0.5f;
      color = _color;
    }

    void render(color_shader &shader, mat4t &cameraToWorld) {

      mat4t modelToProjection = mat4t::build_projection_matrix(modelToWorld, cameraToWorld);
      shader.render(modelToProjection, color);

      float vertices[] = {
        -halfWidth, -halfHeight, 0,
        halfWidth, -halfHeight, 0,
        halfWidth,  halfHeight, 0,
        -halfWidth,  halfHeight, 0,
      };

      glVertexAttribPointer(attribute_pos, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)vertices );
      glEnableVertexAttribArray(attribute_pos);
      glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    void translate(float x, float y) {
      modelToWorld.translate(x, y, 0);
    }

    void set_relative(block &rhs, float x, float y) {
      modelToWorld = rhs.modelToWorld;
      modelToWorld.translate(x, y, 0);
    }

    bool collides_with(const block &rhs) const {
      float dx = rhs.modelToWorld[3][0] - modelToWorld[3][0];
      float dy = rhs.modelToWorld[3][1] - modelToWorld[3][1];
      return
        (fabsf(dx) < halfWidth + rhs.halfWidth) &&
        (fabsf(dy) < halfHeight + rhs.halfHeight);
    }
  };




  class snake_app : public octet::app {

  public:

    mat4t cameraToWorld;
    color_shader color_shader_;

    enum state_t {
      state_moving_up,
      state_moving_down,
      state_moving_left,
      state_moving_right
    };
    state_t state;

    int scores[2];
    float snake_velocity;

    float courtLength;
    float courtThickness;
    float bGBlockDimension;
    float bGBlockGap;

    enum {
      numberOfRows = 11,
      numberOfCols = 11,
      numberOfBGBlocks = numberOfRows*numberOfCols,
    };

    block bGBlock[numberOfBGBlocks];
    block court[4];


    struct linkedList {

      struct item {
        item* next;
        item* prev;
        block snakeBlock;
        int x;
        int y;
      };

      item head;
      item* emptyItem;

      linkedList() {
        head.next = &head;
        head.prev = &head;
        emptyItem = NULL;
      }

      item* newItem(item *prev) {

        float bGBlockDimension = 0.5f;
        float bGBlockGap = 0.1f;

        item *next = prev->next;
        item *current = new item;
        current->next = prev->next;
        current->prev = prev;
        current->next->prev = current;
        prev->next = current;
        current->x = prev->x -1;
        current->snakeBlock.init(vec4(0, 1, 0, 1),
          (bGBlockDimension + bGBlockGap)*((float)current->x - (float)(numberOfCols-1)*0.5f),
          (bGBlockDimension + bGBlockGap)*((float)current->y - (float)(numberOfRows-1)*0.5f),
          bGBlockDimension, bGBlockDimension);
        return current;
      }
    };

    linkedList snakeList;

    void simulate() {

      if (is_key_down(key_left)) {
        snakeList.head.snakeBlock.translate(-(bGBlockDimension + bGBlockGap), 0);
      }

      if (is_key_down(key_right)) {
        snakeList.head.snakeBlock.translate((bGBlockDimension + bGBlockGap), 0);
      } 

      if (is_key_down(key_up)) {
        snakeList.head.snakeBlock.translate(0, (bGBlockDimension + bGBlockGap));
      }

      if (is_key_down(key_down)) {
        snakeList.head.snakeBlock.translate(0, -(bGBlockDimension + bGBlockGap));
      }

      if (is_key_down(key_space)) {
        snakeList.emptyItem = snakeList.newItem(&snakeList.head);

      }

      /*

      //  if (state == state_serving_left) {
      // if we are serving, glue the ball to the left bat
      //    ball.set_relative(bat[0], 0.3f, 0);
      //    if (is_key_down(key_space)) {
      // space serves, changing the state
      //      state = state_playing;
      //      ball_velocity_x = 0.1f;
      //      ball_velocity_y = 0.1f;
      }
      //  } else if (state == state_serving_right) {
      // if we are serving, glue the ball to the right bat
      //    ball.set_relative(bat[1], -0.3f, 0);
      //    if (is_key_down(key_space)) {
      // space serves, changing the state
      //      state = state_playing;
      //      ball_velocity_x = -0.1f;
      //      ball_velocity_y = 0.1f;
      }
      //  } else if (state == state_playing) {
      // if we are playing, move the ball
      //    ball.translate(ball_velocity_x, ball_velocity_y);

      // check collision with the bats
      //    if (ball_velocity_x > 0 && ball.collides_with(bat[1])) {
      // to avoid internal bounces, only check the bats
      //      ball_velocity_x = -ball_velocity_x;
      //    } else if (ball_velocity_x < 0 && ball.collides_with(bat[0])) {
      //      ball_velocity_x = -ball_velocity_x;
      }

      // check collision with the court top and bottom
      //    if (ball_velocity_y > 0 && ball.collides_with(court[1])) {
      //      ball_velocity_y = -ball_velocity_y;
      //    } else if (ball_velocity_y < 0 && ball.collides_with(court[0])) {
      //      ball_velocity_y = -ball_velocity_y;
      }

      // check collision with the court end zones
      //    if (ball.collides_with(court[2])) {
      //      scores[0]++;
      //      state = scores[0] >= 10 ? state_game_over : state_serving_left;
      //    } else if (ball.collides_with(court[3])) {
      //      scores[1]++;
      //      state = scores[1] >= 10 ? state_game_over : state_serving_right;
      }
      */
    }


  public:

    snake_app(int argc, char **argv) : app(argc, argv) {
    }

    void app_init() {
      color_shader_.init();
      cameraToWorld.loadIdentity();
      cameraToWorld.translate(0, 0, 5);

      courtLength = 8.0f;
      courtThickness = 0.2f;
      bGBlockDimension = 0.5f,
       bGBlockGap = 0.1f,

      court[0].init(vec4(1, 1, 0, 1), 0, -4, courtLength + courtThickness, courtThickness);
      court[1].init(vec4(1, 1, 0, 1), 0,  4, courtLength + courtThickness, courtThickness);
      court[2].init(vec4(1, 1, 0, 1), -4, 0, courtThickness, courtLength + courtThickness);
      court[3].init(vec4(1, 1, 0, 1), 4,  0, courtThickness, courtLength + courtThickness);

      snakeList.head.x = (int)(numberOfRows/2);
      snakeList.head.y = (int)(numberOfCols/2);
      snakeList.head.snakeBlock.init(vec4(0, 1, 0, 1),
        (bGBlockDimension + bGBlockGap)*((float)snakeList.head.x - (float)(numberOfCols-1)*0.5f),
        (bGBlockDimension + bGBlockGap)*((float)snakeList.head.y - (float)(numberOfRows-1)*0.5f),
        bGBlockDimension, bGBlockDimension);

      for (int j = 0; j < numberOfRows; ++j) {
        for (int i = 0; i < numberOfCols; ++i) {
          bGBlock[j*numberOfCols + i].init(vec4(0.2f, 0.2f, 0.2f, 1),
            (bGBlockDimension + bGBlockGap)*((float)i - (float)(numberOfCols-1)*0.5f),
            (bGBlockDimension + bGBlockGap)*((float)j - (float)(numberOfRows-1)*0.5f),
            bGBlockDimension, bGBlockDimension);
        }
      }

    }




    void draw_world(int x, int y, int w, int h) {

      simulate();
      glViewport(x, y, w, h);
      glClearColor(0, 0, 0, 1);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);

      for (int i = 0; i != 4; ++i) {
        court[i].render(color_shader_, cameraToWorld);
      }

      snakeList.head.snakeBlock.render(color_shader_, cameraToWorld);
      if (snakeList.emptyItem) {
        snakeList.emptyItem->x = 5;
      }
      //    snakeList.emptyItem->snakeBlock.render(color_shader_, cameraToWorld);

      for (int i = 0; i < numberOfBGBlocks; ++i) {
        bGBlock[i].render(color_shader_, cameraToWorld);
      }
    }

  };

}
