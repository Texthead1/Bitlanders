# Building with VSCode
The `build.bat` requires command line arguments to build the ROM. Like so:
```
build.bat <project_name> <mapper> <output_rom_name> [--verbose]
```

The first argument, `<project_name>`, guides the engine to target a specific project inside of the `projects` folder, and so the string must match up with one of those folders. For this example, we'll be building "bitlanders", and so we will use that as our first argument.

The second argument, `<mapper>`, is which mapper we want to build our project with. There are 3 available mappers in the engine as of writing:
- NROM
- CNROM
- MMC3

For this example, we will build with "MMC3", as it is the ony mapper Bitlanders currently supports.

The third argument, `<output_rom_name>`, is the name of the output file. In this example, we'll simply call it "game.nes".

There is an optional fourth option to enable verbose logging to the terminal, which provides additional information about the building process and what files it discovers. This is by no means necessary to use. In this example, we'll skip it.

Great! So now if we want to build our project, our arguments would look like so:
```
build.bat bitlanders mmc3 game.nes
```

And this will work completely fine, but might get a little tedious if we need to manually type this in for each new build. Luckily, we can save ourselves the hassle and automate thw peocess with Visual Studio Code's build tasks. This will allow us to create a dropdown menu inside of the IDE to build the ROM with a mapper of our choosing.

## Setup
Firstly, a `.vscode` folder needs to be created in the project root. Afterwards, we need to create 2 new files inside this folder. Copy and paste the contents of these code blocks into 2 new respective files, one called `launch.json`, and the other, `tasks.json`.

### `launch.json`
```
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Build",
      "type": "cppvsdbg",
      "request": "launch",
      "program": "cmd.exe",
      "args": ["/c", "exit"],
      "cwd": "${workspaceFolder}",
      "console": "integratedTerminal",
      "preLaunchTask": "Compile"
    }
  ]
}
```

### `tasks.json`
At the moment, we'll just build "Bitlanders" by default. You could create a unique task or dropdown for each project you have.
```
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Compile",
      "type": "shell",
      "command": "${workspaceFolder}/build.bat",
      "problemMatcher": [],
      "args": [
        "bitlanders",
        "${input:mapper}",
        "game.nes"
      ]
    }
  ],
  "inputs": [
    {
      "id": "mapper",
      "description": "Mapper:",
      "default": "MMC3",
      "type": "pickString",
      "options": [
          "NROM",
          "CNROM",
          "MMC3"
      ]
    },
  ]
}
```

The directory should now include the files we newly created, like so:

![VSCode Build Task directory](/assets/media/vscode_build_dir.png)

> [!NOTE]
> These files appear grayed out due to the `.gitignore` file in this project. As a result, these files won't be pushed when creating a new commit, and will stay only on your local machine.

The `inputs` **pickString** we've created in the task will cause a dropdown menu to appear in VSCode when we run the task. This will allow us to choose the mapper, which will act as the command line argument dictating which mapper to pass into the `build.bat`.

![VSCode Build Task mapper options](/assets/media/vscode_build_task.png)

## Results
Now we can run a task to build the ROM. This can be done by pressing `F5` to start debugging, or going to `Terminal > Run Task... >` and selecting the task. In this example, we named the task "Compile":

![VSCode Building](/assets/media/vscode_building.gif)

The ROM will be placed in a `build` folder within the actual project directory under the filename we chose. In this example, our ROM would be built to `projects/bitlanders/build/game.nes`.

## Future Notes
In future, it is likely that the Visual Studio Code task for building each project would be located inside of the project directory itself, instead of the engine, as it doesn't make sense to have a build task for each project inside of the engine itself when the IDE would likely be open in each independent project. The current approach is mostly a carry-over from before the game engine and projects were separate, and so it made sense to put all of the build resources here.