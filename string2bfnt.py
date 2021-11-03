string = """String goes here
"""
output = ""
cnt = 0
for char in string:
    cnt = 0
    for i in bin(ord(char))[2:]:
        output += ">"
        if i == "1":
            output += "~"
        cnt += 1
    output += "+" * cnt + "<" * cnt + '.' + '&' + "-" * cnt
    
print(output[:-1-cnt])
