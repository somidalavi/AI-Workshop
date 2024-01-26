import itertools
import functools
from queue import Queue
import numpy as np

import math
should_log = False
def logistic(x):
    return 1 / (math.e**(-9.3*(x - 3)) + 1)

def ghost_modifier(g1d,g2d,s):
    if g1d >= 2 and g2d >= 2 : return s
    if g1d <= 1 or g2d <= 1 : return 0
    else : return s//2
    
def manhat_dist(x,y):
    return abs(x[0]-y[0]) + abs(x[1]-y[1])
def evalboard(board,pc,gs):
    
    visited = np.zeros(shape=(11,19))
    dist = np.zeros(shape=(11,19))
    global xd,yd
    
    nearest = 1000000
    num_fruit = 0
    fruits = []
    q = Queue()
    q.put((pc,0))
    visited[pc[0]][pc[1]] = 1
    while not q.empty():
        v , d = q.get()
        if board[v[0]][v[1]] == b'.':
            nearest = min(nearest,d)
            num_fruit += 1
            fruits.append(d)
        vi, vj = v
        for i in range(4):
            ci = vi + yd[i]
            cj = vj + xd[i]
            if (board[ci][cj] == b'-') : continue
            if visited[ci][cj] == 1: continue
            dist[ci][cj] = d+1
            visited[ci][cj] = 1
            q.put(((ci,cj),d+1))
            
    g1d = dist[gs[0][0]][gs[0][1]]
    g2d = dist[gs[1][0]][gs[1][1]]
    g1d, g2d = max(g1d,g2d) , min(g1d,g2d)
    for i in range(1,len(fruits)):
        fruits[i] = fruits[i- 1] + 1 if fruits[i] - fruits[i - 1] <= 1 else fruits[i]
        
    predicted_steps = ghost_modifier(g1d,g2d,fruits[-1])
    have_eaten = 0
    have_moved = 0
    for i in range(len(fruits)):
        if fruits[i] > predicted_steps:
            break
        have_eaten += 1
        have_moved = fruits[i]
    if should_log:
        for i, row in enumerate(board):
            for j ,col in enumerate(row):
                c = col.decode('UTF-8')
                if i == gs[0][0] and j == gs[0][1] : c = 'g'
                if i == gs[1][0] and j == gs[1][1] : c = 'g' 
                print(c,end="")
            print('')
        print(have_eaten * 10 - 5* have_moved - 20 * nearest + min(g1d + g2d,5),have_eaten,have_moved,nearest,g1d,g2d)
    return have_eaten * 10 - 5* have_moved - 20 * nearest + min(g1d + g2d,5)
    
def minmax(board,pc,gs,mfunc='max',currentpoints = 0,depth = 0):
    #print(gs)
    if pc[0] == gs[0][0] and pc[1] == gs[0][1] : return 10,currentpoints
    if pc[0] == gs[1][0] and pc[1] == gs[1][1] : return 10,currentpoints
    global max_d
    global yd,xd
    if depth == max_d :
        return 0, evalboard(board,pc,gs) + currentpoints
    best = None
    bdir = None
    if mfunc == 'max':
        for dir in range(5):
            i,j = pc[0] + yd[dir],pc[1] + xd[dir]
            if board[i][j] == b'-': continue
            dboard = board.copy()
            nextpoints = currentpoints - 1
            if dboard[i][j] == b'.' :
                nextpoints += 10
                dboard[i][j] = b','
                
            d , ceval = minmax(dboard,(i,j),gs,'min',nextpoints,depth+1)
            if best == None or ceval > best:
                best = ceval
                bdir = dir
    elif mfunc == 'min' :
        possible = 0
        best = 0
        for dir1, dir2 in itertools.product(range(5),range(5)):
            g1i , g1j = gs[0][0] + yd[dir1],gs[0][1] + xd[dir1]
            g2i , g2j = gs[1][0] + yd[dir2],gs[1][1] + xd[dir2]
            if board[g1i][g1j] == b'-' : continue
            if board[g2i][g2j] == b'-' : continue
            possible += 1
            d , ceval = minmax(np.copy(board),pc,((g1i,g1j),(g2i,g2j)), 'max',currentpoints , depth+ 1)
            best += ceval
        best /= possible 
    return bdir,best
        
with open("board.txt") as f:
    buff = "".join(f.readlines())
    print(buff.count('.'))
    buff = buff.replace("\n","")
    print(len(buff))
    print(buff.encode())
    print(len(buff.encode()))
    board = np.chararray(shape=(11,19),buffer=buff.encode())
    
print(board)
pacmans = np.where(board == b'c')
ghostss = np.where(board == b'g')
pacman = [pacmans[0][0],pacmans[1][0]]
ghosts = [[ghostss[0][0],ghostss[1][0]],[ghostss[0][1],ghostss[1][1]]]
board = np.copy(board)
board[ghosts[0][0]][ghosts[0][1]] = '.'
board[ghosts[1][0]][ghosts[1][1]] = '.'
points = 0
moves = 0
print(ghosts)
print(pacman)
xd = [0,1,0,-1,0]
yd = [-1,0,1,0,0]
max_d = 2
curmove = 0
        
dirs = ['up','right','down','left',"stop"]
curpoints = 0
eaten = 0
import math
while True:
    
    
    d , p = minmax(board,pacman,ghosts,currentpoints=curpoints,mfunc='max',depth=0)
    if d == 10:
        print("gameover") 
        print("points",p)
        exit()
    
    print("prediction",p)
    print("dir ",dirs[d],d)
    board[pacman[0]][pacman[1]] = b','
    pacman[0] += yd[d]
    pacman[1] += xd[d]
    gdir1 = np.random.randint(0,4)
    gdir2 = np.random.randint(0,4)
    g1y = ghosts[0][0] + yd[gdir1]
    g1x = ghosts[0][1] + xd[gdir1]
    g2y = ghosts[1][0] + yd[gdir2]
    g2x = ghosts[1][1] + xd[gdir2]
    print(ghosts)
    print(g1y,g1x,g2y,g2x)
    if board[g1y][g1x] != b'-':
        ghosts[0] = [g1y,g1x]
    if board[g2y][g2x] != b'-':
        ghosts[1] = [g2y,g2x]
    
   #if board[pacman[0]][pacman[1]] == b',' and curpoints > 350:
    #    should_log = True
        
    if board[pacman[0]][pacman[1]] == b'.':
        eaten += 1
        curpoints += 10
        
    board[pacman[0]][pacman[1]] = b'c'
    curpoints -= 1
    print("pacman at",pacman)
    print("ghosts at " ,ghosts)
    print("current board")
    for i, row in enumerate(board):
        for j ,col in enumerate(row):
            c = col.decode('UTF-8')
            if i == ghosts[0][0] and j == ghosts[0][1] : c = 'g'
            if i == ghosts[1][0] and j == ghosts[1][1] : c = 'g' 
            print(c,end="")
        print('')
    print("current points",curpoints, eaten)
    #x = input("continue....?")
    x = 'w'
    curmove += 1
    if x == "Q":
        exit()