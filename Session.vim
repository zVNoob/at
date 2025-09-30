let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/at
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +13 main.cpp
badd +1 lexer.hpp
badd +102 lexer.cpp
badd +1 parser.y
badd +1 error.hpp
badd +29 error.cpp
badd +5 objects/object.hpp
badd +12 objects/type.hpp
badd +8 objects/callable/callable.hpp
badd +5 objects/callable/internal_func.hpp
badd +13 objects/callable/internal_func.cpp
badd +17 objects/callable/typed_func.hpp
badd +9 objects/callable/typed_func.cpp
badd +13 objects/primitives/integer.hpp
badd +22 parser_helper.hpp
badd +9 parser_helper.cpp
badd +13 objects/primitives/string.hpp
badd +15 objects/primitives/string.cpp
badd +10 objects/primitives/fraction.hpp
badd +7 objects/primitives/fraction.cpp
badd +118 type/bigfraction.hpp
badd +1 objects/variable.hpp
badd +34 Makefile
badd +21 objects/scope.hpp
badd +16 objects/scope.cpp
badd +1 objects/container/array.hpp
badd +22 objects/container/array.cpp
badd +1 objects/container
badd +21 objects/container/tuple.cpp
argglobal
%argdel
$argadd main.cpp
set stal=2
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit main.cpp
argglobal
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 8 - ((7 * winheight(0) + 28) / 57)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 8
normal! 0
tabnext
edit lexer.hpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 57 + 30) / 60)
exe 'vert 1resize ' . ((&columns * 118 + 118) / 237)
exe '2resize ' . ((&lines * 57 + 30) / 60)
exe 'vert 2resize ' . ((&columns * 118 + 118) / 237)
argglobal
balt lexer.cpp
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 1 - ((0 * winheight(0) + 28) / 57)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 012|
wincmd w
argglobal
if bufexists(fnamemodify("lexer.cpp", ":p")) | buffer lexer.cpp | else | edit lexer.cpp | endif
if &buftype ==# 'terminal'
  silent file lexer.cpp
endif
balt lexer.hpp
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 102 - ((0 * winheight(0) + 28) / 57)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 102
normal! 053|
wincmd w
exe '1resize ' . ((&lines * 57 + 30) / 60)
exe 'vert 1resize ' . ((&columns * 118 + 118) / 237)
exe '2resize ' . ((&lines * 57 + 30) / 60)
exe 'vert 2resize ' . ((&columns * 118 + 118) / 237)
tabnext
edit parser.y
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
wincmd _ | wincmd |
split
1wincmd k
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 57 + 30) / 60)
exe 'vert 1resize ' . ((&columns * 118 + 118) / 237)
exe '2resize ' . ((&lines * 27 + 30) / 60)
exe 'vert 2resize ' . ((&columns * 118 + 118) / 237)
exe '3resize ' . ((&lines * 29 + 30) / 60)
exe 'vert 3resize ' . ((&columns * 118 + 118) / 237)
argglobal
balt parser_helper.hpp
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 64 - ((10 * winheight(0) + 28) / 57)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 64
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("parser_helper.hpp", ":p")) | buffer parser_helper.hpp | else | edit parser_helper.hpp | endif
if &buftype ==# 'terminal'
  silent file parser_helper.hpp
endif
balt parser_helper.cpp
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 22 - ((10 * winheight(0) + 13) / 27)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 22
normal! 032|
wincmd w
argglobal
if bufexists(fnamemodify("parser_helper.cpp", ":p")) | buffer parser_helper.cpp | else | edit parser_helper.cpp | endif
if &buftype ==# 'terminal'
  silent file parser_helper.cpp
endif
balt parser_helper.hpp
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 22 - ((10 * winheight(0) + 14) / 29)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 22
normal! 016|
wincmd w
exe '1resize ' . ((&lines * 57 + 30) / 60)
exe 'vert 1resize ' . ((&columns * 118 + 118) / 237)
exe '2resize ' . ((&lines * 27 + 30) / 60)
exe 'vert 2resize ' . ((&columns * 118 + 118) / 237)
exe '3resize ' . ((&lines * 29 + 30) / 60)
exe 'vert 3resize ' . ((&columns * 118 + 118) / 237)
tabnext
edit error.hpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 57 + 30) / 60)
exe 'vert 1resize ' . ((&columns * 118 + 118) / 237)
exe '2resize ' . ((&lines * 57 + 30) / 60)
exe 'vert 2resize ' . ((&columns * 118 + 118) / 237)
argglobal
balt error.cpp
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 30 - ((29 * winheight(0) + 28) / 57)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 30
normal! 03|
wincmd w
argglobal
if bufexists(fnamemodify("error.cpp", ":p")) | buffer error.cpp | else | edit error.cpp | endif
if &buftype ==# 'terminal'
  silent file error.cpp
endif
balt error.hpp
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 29 - ((28 * winheight(0) + 28) / 57)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 29
normal! 030|
wincmd w
exe '1resize ' . ((&lines * 57 + 30) / 60)
exe 'vert 1resize ' . ((&columns * 118 + 118) / 237)
exe '2resize ' . ((&lines * 57 + 30) / 60)
exe 'vert 2resize ' . ((&columns * 118 + 118) / 237)
tabnext
edit objects/variable.hpp
argglobal
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 1 - ((0 * winheight(0) + 28) / 57)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
tabnext
edit objects/container/array.hpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
wincmd =
argglobal
balt objects/container/array.cpp
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 12 - ((11 * winheight(0) + 29) / 58)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 12
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("objects/container/array.cpp", ":p")) | buffer objects/container/array.cpp | else | edit objects/container/array.cpp | endif
if &buftype ==# 'terminal'
  silent file objects/container/array.cpp
endif
balt objects/container/array.hpp
setlocal foldmethod=manual
setlocal foldexpr=0
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=0
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
silent! normal! zE
let &fdl = &fdl
let s:l = 22 - ((21 * winheight(0) + 29) / 58)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 22
normal! 016|
wincmd w
2wincmd w
wincmd =
tabnext 6
set stal=1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
