# Building with VSCode
The `build.bat` requires a command line argument to build the ROM, the argument being which mapper to select. There are 3 available mappers in the engine as of writing:
- NROM
- CNROM
- MMC3

If you do not want to open a terminal to then run the batch file, the process can be automated using Visual Studio Code's build tasks. This will allow us to create a dropdown menu inside of the IDE to build the ROM with a mapper of our choosing.

## Set up
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
```
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Compile",
      "type": "shell",
      "command": "${workspaceFolder}/build.bat",
      "problemMatcher": [],
      "args": ["${input:mapper}"]
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

The directory should now look like so:

![VSCode Build Task directory](/assets/media/vscode_build_dir.png)

> [!NOTE]
> These files appear grayed out due to the `.gitignore` file in this project. As a result, these files won't be pushed when creating a new commit, and will stay only on your local machine.

The `inputs` **pickString** we've created in the task will cause a dropdown menu to appear in VSCode when we run the task. This will allow us to choose the mapper, which will act as the command line argument dictating which mapper to pass into the `build.bat`.

![VSCode Build Task mapper options](/assets/media/vscode_build_task.png)

## Results
Now we can run a task to build the ROM. This can be done by pressing `F5` to start debugging, or going to `Terminal > Run Task... >` and selecting the task. In this example, we named the task "Compile":

![VSCode Building](/assets/media/vscode_building.gif)