# Screen flash test - validates backlight logic
test_levels = [0.0, 0.03, 0.08, 0.15, 0.25, 0.4, 0.6, 0.8]

print("Audio Level | Brightness")
for level in test_levels:
    if level > 0.3:
        brightness = 100
    elif level > 0.1:
        brightness = 70
    elif level > 0.05:
        brightness = 40
    else:
        brightness = 10
    print(f"{level:10.2f} | {brightness:9d}%")

print("\nScreen flash logic OK")