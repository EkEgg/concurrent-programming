//introduz canais na aplicação Go
package main

import (
	"fmt"
)

func Chat(chattingChannel chan string) {

    const ReceiveTemplate = "[Goroutine received from Main]: %s\n"

	message := <- chattingChannel

	fmt.Printf(ReceiveTemplate, message)

	chattingChannel <- "Howdy, Main! All good?"

    message = <- chattingChannel

    fmt.Printf(ReceiveTemplate, message)

    chattingChannel <- "Alright, bro, I got you!"

	fmt.Printf("[Goroutine left]\n")
    chattingChannel <- ""
}

func main() {

    const ReceiveTemplate = "[Main received from Goroutine]: %s\n"

	chattingChannel := make(chan string)
	
	go Chat(chattingChannel)

	chattingChannel <- "Sup, Goroutine!"

	message := <- chattingChannel

	fmt.Printf(ReceiveTemplate, message)

    chattingChannel <- "Sure, man. I'll get going, 'kay?"

    message = <- chattingChannel

    fmt.Printf(ReceiveTemplate, message)

    <- chattingChannel

    fmt.Println("[Main left]\n")
}
