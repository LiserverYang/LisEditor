# LisEditor

[简体中文](./README_ZHCN.md)

This is a easy open-source text editor repository.

<br>
<hr>
<br>

## Build

- Windows:
    ```shell
    .\build.cmd
    ```
    or
    ```shell
    g++ .\editor.cpp -std=c++11 -Werror -o liseditor
    ```

- MacOS or Linux:
    It can't build on MacOS or Linux. But you also can use wine.

<br>

## How to use

<br>

**./liseditor** **./a.cpp[FileName]** **-ro[readonly]**

- Press key **CTRL + S** to save change.
- Press key **CTRL + C** to exit this application.