

LOWER_LENGTH = 3.0

lower_bar = box(
    pos=base_pivot.pos + vector(LOWER_LENGTH / 2, 0, 0),
    size=vector(LOWER_LENGTH, 0.45, 0.45),
    axis=vector(LOWER_LENGTH, 0, 0),
    up=vector(0, 1, 0)
)



upper_joint = sphere(
    pos=base_pivot.pos + vector(LOWER_LENGTH, 0, 0),
    radius=0.35
)


UPPER_LENGTH = 3.0

upper_bar = box(
    pos=upper_joint.pos + vector(UPPER_LENGTH / 2, 0, 0),
    size=vector(UPPER_LENGTH, 0.45, 0.45),
    axis=vector(UPPER_LENGTH, 0, 0),
    up=vector(0, 1, 0)
)


HOST = "127.0.0.1"
PORT = 5000

server = socket.socket(
    socket.AF_INET,
    socket.SOCK_STREAM
)

server.bind((HOST, PORT))
server.listen(1)

print("Robot TaiJi simulator started.")
print("Waiting for controller...")

connection, address = server.accept()

print("Controller connected!")
print(address)

buffer = ""


while True:

    rate(60)

    data = connection.recv(4096)

    if not data:
        break

    buffer += data.decode()

    messages = buffer.split("\n")

    buffer = messages[-1]

    for message in messages[:-1]:

        message = message.strip()

        if not message:
            continue

        try:

        

            lower, upper = map(
                int,
                message.split(",")
            )

            lower = radians(lower)
            upper = radians(upper)

            

            lower_direction = vector(1, 0, 0)

            lower_direction = lower_direction.rotate(
                angle=lower,
                axis=vector(0, 0, 1)
            )

            lower_bar.axis = (
                lower_direction * LOWER_LENGTH
            )

            lower_bar.pos = (
                base_pivot.pos
                + lower_direction *
                (LOWER_LENGTH / 2)
            )

           

            upper_joint.pos = (
                base_pivot.pos
                + lower_direction * LOWER_LENGTH
            )

         
            upper_direction = vector(1, 0, 0)

            upper_direction = upper_direction.rotate(
                angle=upper,
                axis=vector(0, 1, 0)
            )

            upper_bar.axis = (
                upper_direction * UPPER_LENGTH
            )

            upper_bar.up = vector(0, 1, 0)

            upper_bar.pos = (
                upper_joint.pos
                + upper_direction *
                (UPPER_LENGTH / 2)
            )

        except ValueError:

            print(
                "Invalid message:",
                message
            )


while True:
    rate(30)
