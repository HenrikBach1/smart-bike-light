package com.example.fekotlinmp.shared

actual class Platform {
    actual val name: String = "Android"
}

actual fun platformName(): String = "Android"