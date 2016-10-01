# .screenrc
hardstatus alwayslastline
hardstatus string '%{= kG}[ %{G}%H %{g}][%= %{= kw}%?%-Lw%?%{r}(%{W}%n*%f%t%?(%u)%?%{r})%{w}%?%+Lw%?%?%= %{g}][%{B} %m/%d %{W}%c %{g}]'

# explanation of hardstatus string: 
# 1st segment: change attr,black/brightGreen,[,brightGreen,$HOSTNAME,green,]
# 2nd segment: [,center following,

startup_message off
defscrollback 200000

#emacs stuff:
bindkey -k kb
bindkey -d -k kb
#
## do not trash Delete, usually ESC [ 3 ~
bindkey -k kD
bindkey -d -k kD

#evil screen steals my ^A, so this moves it
escape ""


#mousetrack on
#caption always
vbell off
caption string "%?%f%{= bk}%? %c %D %m-%d-%y %{= kb} %t%= %?%f%{=bk}%:%{= wk}%? %n "
caption splitonly
#caption always "%n(%t) : %C"
#
#
#Slightly more complex would be:
#
#caption always "%{= kc}%H (system load: %l)%-21=%{= .m}%D %d.%m.%Y %0c"

termcapinfo xterm-color ti@:te@

##  Toggle 'fullscreen' or not.
##
#bind f eval "caption splitonly" "hardstatus ignore"
#bind F eval "caption always"    "hardstatus alwayslastline"
bind f eval "hardstatus ignore" #splitonly"
bind F eval "hardstatus alwayslastline"

#Here are several useful resizing commands for a split screen.
bind = resize =  # make split windows equal
bind + resize +1 # increase current window size by one
bind ` resize +10 # increase current window size by one
bind ~ resize -10 # increase current window size by one
bind - resize -1 # decrease current window size by one
bind _ resize max  # maximize the current window split
