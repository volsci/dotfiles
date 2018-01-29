set nocompatible 
filetype off
:set cursorline


call plug#begin()
	Plug 'morhetz/gruvbox'
	Plug 'vim-airline/vim-airline'
	Plug 'vim-airline/vim-airline-themes'
    Plug 'tmhedberg/SimpylFold'
    Plug 'scrooloose/nerdtree'
    Plug 'tpope/vim-fugitive'
    Plug 'LucHermitte/lh-vim-lib'
    Plug 'LucHermitte/local_vimrc'
    Plug 'reedes/vim-pencil'
    Plug 'itchyny/vim-cursorword'
call plug#end()

let g:gruvbox_italic=1
let g:gruvbox_italicize_comments = 1
let g:gruvbox_italicize_strings = 1 

let g:airline_powerline_fonts = 1

set foldmethod=indent
set foldlevel=99
nnoremap <space> za

syntax enable
set tabstop=4
set softtabstop=4
set expandtab

set wrap
set ai
set linebreak
set nolist
set textwidth=0
set wrapmargin=0
set formatoptions-=t
set formatoptions+=l
set shiftwidth=4
let mapleader=","
set spell

set showcmd
set cursorline
set showmatch
autocmd vimenter * NERDTree

set incsearch
set hlsearch

set background=light

let g:pencil#wrapModeDefault = 'hard'   " default is 'hard'
let g:gruvbox_contrast_light = 'medium'
set cursorline

colorscheme gruvbox
