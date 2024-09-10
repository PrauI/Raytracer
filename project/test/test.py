import os

executable = "./main.exe"

# Ensure the executable has execute permissions
os.chmod(executable, 0o755)

# Folder of input tests
folder_path = "test/inputs"
file_list = os.listdir(folder_path)

# Loop through the list and print each file name
iteration = 0
for file_name in file_list:
    print(f"Running test {iteration}:")
    iteration += 1
    input = f"{folder_path}/{file_name}"
    output = f"test/outputs/{file_name[:-4]}png"
    command = f"{executable} {output} {input}"
    print(f"Running command: {command}")
    os.system(command)
    print('\n')
    for c in range(100):
        print("-", end="")
    print('\n')