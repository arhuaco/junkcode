#!/usr/bin/python

import seesound, sys, pygame, gd, time, random, thread
#from pygame.locals import *

class SoundMatrix:
  "Representation that will be translated to sound"

  def __init__ (self, size):

    self.size = size
    self.arr = []
    self.sound = seesound.SeeSound(500, 5000, 'e', self.size)

    for i in xrange(0, self.size):
      self.arr.append([])
      for j in xrange(0, self.size):
        self.arr[i].append(0)

  def make_sound(self, filename): # Do not use concurrently
    for i in xrange(0, self.size):
      for j in xrange(0, self.size):
        self.sound.set(i,j,int(self.arr[i][j]))
    self.sound.write(filename)

def render_image(file, levels, zoom):
  img = gd.image(file)

  def pixel_value(row,col):
    c = (img.colorComponents(img.getPixel((row,col)))) # get RGB
    c =  (c[0] + c[1] + c[2]) / 3 # Grayscale
    return int(c), levels - c * (float(levels - 1) / 256.0) - 1 # grayscale, normalized

  w, h = img.size()

  surf = pygame.Surface((w * zoom, h * zoom))

  arr = []
  for i in xrange(0, h):
    arr.append([])
    for j in xrange(0, w):
      arr[i].append([])
      c, arr[i][j] = pixel_value(j,i)
      pygame.draw.rect(surf, (c,c,c), (j * zoom, i * zoom, zoom, zoom))

  return surf, arr, w, h

class MovingObject:

  def __init__ (self, imagefile, levels, zoom):
    self.surface, self.array_repr, self.width, self.height =  render_image(imagefile, levels, zoom)
    self.posx = 0
    self.posy = 0
    self.speedx = 0
    self.speedy = 0

  def max_posx(self, size):
    return size - self.width

  def max_posy(self, size):
    return size - self.height

  def get_rect(self):
    return pygame.Rect(self.posx, self.posy, self.width,self.height) 

  def step(self, size, elapsedtime):
    if elapsedtime > 0:
      self.posx += self.speedx * elapsedtime 
      self.posy += self.speedy * elapsedtime 

    max_posx = self.max_posx(size)
    max_posy = self.max_posy(size)

    if self.posy > max_posy:
      self.posy = max_posy
    elif self.posy < 0:
      self.posy = 0

    if self.posx > max_posx:
      self.posx = max_posx
    elif self.posx < 0:
      self.posx = 0

class FallingGame:

  actors = []
  falling = []
  time = None
  
  fell = 0
  catched = 0
  perfect = 0

  def __init__ (self, levels, size, zoom):

    self.levels = levels
    self.zoom = zoom
    self.size = size
    self.cursor_speed = 5
    self.cursor = self.AddCursor()
    self.paused = False

    self.AddFalling()

  def AddFalling(self):
  
    o = MovingObject('img/ball.png', self.levels, self.zoom)
    self.actors.append(o)
    self.falling.append(o)
    self.placeFalling(o)

  def placeFalling(self, o):

        o.posy = 0
        o.posx = random.randint(0, int(o.max_posx(self.size)))
        o.speedy = 5
        o.speedx = 0

  def AddCursor(self):

    o = MovingObject('img/cursor.png', self.levels, self.zoom)
    self.actors.append(o)
    o.posx, o.posy, o.speedx, o.speedy = (0, o.max_posy(self.size), 0, 0)
    return o

  def iterate(self):


    if self.time == None:
      self.time = time.time()

    nexttime = time.time()

    if not self.paused:
      for o in self.actors:
        o.step(self.size, nexttime - self.time)

    self.time = nexttime

    idx = self.cursor.get_rect().collidelist(map(lambda x: x.get_rect(), self.falling))
    if idx != -1:
      perfect = False
      if self.cursor.posx <= self.falling[idx].posx and \
         self.cursor.posx + self.cursor.width >= self.falling[idx].posx + \
         self.falling[idx].width:
        perfect = True

      self.catched += 1

      if perfect:
        self.perfect += 1
        pygame.mixer.Sound("sound/perfect.wav").play()
      else:
        pygame.mixer.Sound("sound/yes.wav").play()

      self.print_score()
      self.placeFalling(self.falling[idx])

    for o in self.falling:
      if o.posy == o.max_posy(self.size):
        self.placeFalling(o)
        self.fell += 1
        pygame.mixer.Sound("sound/no.wav").play()
        self.print_score()


  def read_events(self, events, size, zoom):

    for e in events:
      if e.type == pygame.KEYDOWN:
        if e.key in [pygame.K_LEFT, pygame.K_h]:
          self.cursor.speedx = - self.cursor_speed
        elif  e.key in [pygame.K_RIGHT, pygame.K_l]:
          self.cursor.speedx = self.cursor_speed
        elif e.key in [pygame.K_SPACE, pygame.K_DOWN]:
          self.cursor.speedx = 0
        elif e.key in [pygame.K_j, pygame.K_k, pygame.K_UP]:
          for o in self.falling:
            o.posy = o.max_posy(size)
        elif e.key in [pygame.K_p]:
          self.paused = not self.paused

  def paint(self, screen):

    for o in self.actors:
      screen.blit(o.surface, (o.posx * self.zoom, o.posy * self.zoom))

  def fill_sound(self, snd):

    # clear array
    for i in snd.arr:
      for j in xrange(0, len(i)):
        i[j] = 0

    for o in self.actors:
      for i in xrange(int(o.posy), int(o.posy) + o.height): 
        for j in xrange(int(o.posx), int(o.posx) + o.width): 
          snd.arr[i][j] = o.array_repr[i - int(o.posy)][j - int(o.posx)]

    #def paintxx(x):
    #  if x  > 0:
    #    if x > 5:
    #      return '*'
    #    return '-'
    #  return ' '
    #for i in snd.arr:
    #  print reduce(lambda x, y: x + y, map(paintxx, i), ' ')

  def print_score(self):
    print 'fell:', self.fell, 'catched:', self.catched, 'perfect:', self.perfect, 'score:', self.perfect * 2 + self.catched - self.fell

def main():

    pygame.init()
    random.seed()

    display_flags = pygame.DOUBLEBUF|pygame.RESIZABLE

    size = 64
    levels = 16
    zoom = 3
    width, height = size * zoom, size * zoom

    this_game = FallingGame(levels, size, zoom)

    if pygame.display.mode_ok((width, height), display_flags ):
        screen = pygame.display.set_mode((width, height), display_flags)
    else:
      print >> sys.stderr, "Couldn't initialize the display"
      sys.exit(1)

    run = 1

    clock = pygame.time.Clock()

    pygame.display.set_icon(this_game.cursor.surface)
    pygame.display.set_caption("SeeSound test")

    
    pygame.mixer.init(44100, 16, 2, 1024)
    sound_matrix = SoundMatrix(size) 
    sound_lock   = thread.allocate_lock()

    def write_sound_in_thread():
      sound_matrix.make_sound("test.wav")

      while sound_matrix.sound.is_busy(): # Computing the next sound? Polling!
        time.sleep(0.1)

      pygame.mixer.Sound("test.wav").play()
      sound_lock.release()

    while run:

        events = pygame.event.get()
        for event in events:
            if event.type == pygame.QUIT or (event.type == pygame.KEYDOWN and 
                                      event.key in [pygame.K_ESCAPE, pygame.K_q]):
                run = 0

        this_game.read_events(events, size, zoom)

        screen.fill((255,255,255)) # Is there a better way to clean the screen

        this_game.iterate()
        this_game.paint(screen)

        pygame.display.flip()

        if not sound_lock.locked():
          sound_lock.acquire()
          this_game.fill_sound(sound_matrix)
          thread.start_new_thread(write_sound_in_thread, ())

        clock.tick(60) # limit the game to about 60fps

if __name__ == "__main__":
    main()
