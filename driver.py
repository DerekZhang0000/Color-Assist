import os
import shutil
from time import sleep
import subprocess
import tkinter as tk
from tkinter import ttk, filedialog

from pyparsing import col

def build(vsVersion):
    #Builds C++ executable
    try:
        shutil.rmtree("./build")
    except:
        pass
    while os.path.isdir("./build"):
        sleep(.1)
    os.mkdir("./build")
    def waitForMsg(osPipe, string):
        while string not in osPipe.read():
            sleep(.1)

    os.chdir("build")
    osPipe = os.popen("cmake -S .. -B . -G \"" + vsVersion + "\"")
    waitForMsg(osPipe, "Build files have been written")
    osPipe = os.popen("cmake ..")
    waitForMsg(osPipe, "Build files have been written")
    os.popen("cmake --build .")

    while not os.path.isfile("./Debug/Color-Assist.exe"):
        sleep(.1)
    while(True):
        try:
            os.rename("./Debug/Color-Assist.exe", "./Debug/Color-Assist.exe")
            break
        except:
            sleep(.1)

    os.chdir("..")

def getLocal(path):
    local = path[::-1].split("/")[0][::-1]
    return local

def loadBuildGUI():
    #Load build window
    window = tk.Tk()
    window.geometry("400x300")
    window.tk.call('source', 'azure.tcl')
    window.tk.call('set_theme', 'light')
    style = ttk.Style()
    style.configure("Accent.TButton", foreground="white")
    window.iconbitmap(True, "icon.ico")
    window.title("Color Assist")
    #Dropdown menu
    vsVersions = (
        "Visual Studio 17 2022",
        "Visual Studio 16 2019",
        "Visual Studio 15 2017",
        "Visual Studio 14 2015"
    )
    vsVersion = tk.StringVar()
    vsVersion.set("Visual Studio 17 2022")
    dropdown = ttk.OptionMenu(window, vsVersion, vsVersions[0], *vsVersions)
    dropdown.grid(row=0, column=0, pady=5, columnspan=3, sticky="SW")
    #Build button
    def buildVS():
        build(vsVersion.get())
        window.destroy()
        loadGUI()
    def skipBuild():
        window.destroy()
        loadGUI()

    buildButton = ttk.Button(window, text="Build", command=buildVS, style="Accent.TButton")
    buildButton.grid(row=1, column=0, pady=20, sticky="SW")
    buildButton = ttk.Button(window, text="Skip", command=skipBuild,)
    buildButton.grid(row=1, column=1, padx=10, pady=20, sticky="SW")

    window.mainloop()

def loadGUI():
    global imagePath, downloadPath, boostVar, monoVar, resizeWidth, resizeHeight
    #Load main window
    window = tk.Tk()
    window.geometry("800x600")
    window.tk.call('source', 'azure.tcl')
    window.tk.call('set_theme', 'light')
    style = ttk.Style()
    style.configure("Accent.TButton", foreground="white")
    window.iconbitmap(True, "icon.ico")
    window.title("Color Assist")
    #Set defaults
    imagePath = None
    downloadPath = "./ImageOut"
    boostFactor = .5
    monoFactor = .9
    #Path update buttons
    def updateImage():
        global imagePath
        imagePath = filedialog.askopenfilename()
        imageDisplayPath = getLocal(imagePath)
        imageLabel.config(text="Image(s): " + imageDisplayPath)
    def updateDownload():
        global downloadPath
        downloadPath = filedialog.askdirectory()
        downloadDisplayPath = getLocal(downloadPath)
        downloadLabel.config(text="Download Path: /" + downloadDisplayPath)

    imageLabel = ttk.Label(text="Image:")
    imageLabel.grid(row=0, column=0, padx=35, pady=5, sticky="SW")
    imageButton = ttk.Button(window, text="Choose Image", command=updateImage)
    imageButton.grid(row=1, column=0, padx=35, pady=10, sticky="SW")
    downloadLabel = ttk.Label(text="Download Path: /ImageOut")
    downloadLabel.grid(row=2, column=0, padx=35, pady=5, sticky="SW")
    downloadButton = ttk.Button(window, text="Choose Path", command=updateDownload)
    downloadButton.grid(row=3, column=0, padx=35, pady=10, sticky="SW")

    #Color Assist settings
    boostVar = tk.DoubleVar()
    monoVar = tk.DoubleVar()
    boostVar.set(boostFactor * 100)
    monoVar.set(monoFactor * 100)

    def boostChanged(event):
        boostValueLabel.config(text='{: .2f}'.format(boostVar.get()) + '%')
    def monoChanged(event):
        monoValueLabel.config(text='{: .2f}'.format(monoVar.get()) + '%')

    boostLabel = ttk.Label(text="Red Boost")
    boostLabel.grid(row=0, column=1, padx=35, pady=5, sticky="SW")
    boostSlider = ttk.Scale(window, from_=0, to_=100, orient=tk.HORIZONTAL, command=boostChanged, variable=boostVar)
    boostSlider.grid(row=1, column=1, padx=35, pady=10, sticky="SW")
    boostValueLabel = ttk.Label(text='{: .2f}'.format(boostVar.get()) + '%')
    boostValueLabel.grid(row=1, column=2, padx=5)
    monoLabel = ttk.Label(text="Monochrome Tolerance")
    monoLabel.grid(row=2, column=1, padx=35, pady=5, sticky="SW")
    monoSlider = ttk.Scale(window, from_=0, to_=100, orient=tk.HORIZONTAL, command=monoChanged, variable=monoVar)
    monoSlider.grid(row=3, column=1, padx=35, pady= 10, sticky="SW")
    monoValueLabel = ttk.Label(text='{: .2f}'.format(monoVar.get()) + '%')
    monoValueLabel.grid(row=3, column=2, padx=5)
    resizeWidthLabel = ttk.Label(text="Resize Width")
    resizeWidthLabel.grid(row=4, column=0, padx=35, sticky="SW")
    resizeWidth = ttk.Entry(window)
    resizeWidth.grid(row=5, column=0)
    resizeHeightLabel = ttk.Label(text="Resize Height")
    resizeHeightLabel.grid(row=4, column=1, sticky="SW")
    resizeHeight = ttk.Entry(window)
    resizeHeight.grid(row=5, column=1, sticky="SW")
    #Download button
    def download():
        global imagePath, downloadPath, boostVar, monoVar, resizeWidth, resizeHeight
        try:
            if imagePath != None:
                inputSubstrings = [
                    (downloadPath + "/ColorAssist_" + getLocal(imagePath)).replace(" ", "*"),
                    imagePath.replace(" ", "*"),
                    str(boostVar.get()),
                    str(monoVar.get()),
                ]
                try:
                    if int(resizeWidth.get()) > 0 and int(resizeHeight.get()) > 0:
                        resize = True
                        inputSubstrings += [str(resize), resizeWidth.get(), resizeHeight.get()]
                    else:
                        resize = False
                        inputSubstrings.append(str(resize))
                except:
                    resize = False
                    inputSubstrings.append(str(resize))
                inputString = "?".join(inputSubstrings)
                subprocess.run("./build/Debug/Color-Assist.exe", input=bytes(inputString, "ascii"))
        except:
            pass
                
    downloadButton = ttk.Button(window, text="Download", command=download, style="Accent.TButton")
    downloadButton.grid(row=6, column=0, padx=35, pady=55, sticky="SW")
    window.mainloop()

loadBuildGUI()