import org.jetbrains.kotlin.konan.target.HostManager

plugins {
    kotlin("multiplatform") // Kotlin Multiplatform Plugin
    kotlin("native.cocoapods") // Kotlin Cocoapods Plugin for iOS
    kotlin("plugin.serialization") version "1.9.10" // Serialization Plugin
    id("com.android.library") // Required for Android
}

android {
    namespace = "com.example.fekotlinmp.shared" // Or your app's package/namespace
    compileSdk = 33

    defaultConfig {
        minSdk = 21
        targetSdk = 33 // Will avoid using targetSdkPreview for non-preview targets

        // JVM compatibility is handled in the `kotlin {}` block
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17 // Make sure Java compatibility version matches 17
        targetCompatibility = JavaVersion.VERSION_17
    }
}

kotlin {
    androidTarget() // Replacing deprecated 'android()'
    ios() // Only for macOS builds
    sourceSets {
        val commonMain by getting
        val androidMain by getting {
            dependencies {
                implementation("androidx.core:core-ktx:1.10.1")
            }
        }
    }
}

repositories {
    google()
    mavenCentral()
}