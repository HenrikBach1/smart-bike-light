package com.example.fekotlinmp.shared

expect class Platform {
    val name: String
}

expect fun platformName(): String