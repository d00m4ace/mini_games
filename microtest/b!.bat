call build.bat

copy main.data C:\nginx-1.27.5\html\ /Y
copy main.html C:\nginx-1.27.5\html\ /Y
copy main.js C:\nginx-1.27.5\html\ /Y
copy main.wasm C:\nginx-1.27.5\html\ /Y

del main.data 
del main.html 
del main.js 
del main.wasm 
