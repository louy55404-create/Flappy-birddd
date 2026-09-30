#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

// Konfigurasi Layar
const int SCREEN_WIDTH = 400;
const int SCREEN_HEIGHT = 600;

// Konfigurasi Fisika Burung
const float GRAVITY = 0.25f;
const float JUMP_STRENGTH = -4.5f;

// Konfigurasi Pipa
const float PIPE_SPEED = 2.0f;
const float PIPE_SPAWN_TIME = 1.5f; // dalam detik
const float GAP_SIZE = 150.0f;

struct Pipe {
    sf::RectangleShape topPipe;
    sf::RectangleShape bottomPipe;
    bool passed = false;
};

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Flappy Bird - C++ SFML");
    window.setFramerateLimit(60);

    // Burung
    sf::CircleShape bird(15.0f);
    bird.setFillColor(sf::Color::Yellow);
    bird.setPosition(80.0f, SCREEN_HEIGHT / 2.0f);
    float velocity = 0.0f;

    // Pipa
    std::vector<Pipe> pipes;
    sf::Clock pipeClock;

    // Skor & Status Game
    int score = 0;
    bool gameOver = false;

    // Font dan Teks Skor
    sf::Font font;
    sf::Text scoreText;
    // Menggunakan font standar bawaan sistem atau memuat font kustom jika ada
    if (font.loadFromFile("arial.ttf")) {
        scoreText.setFont(font);
        scoreText.setCharacterSize(30);
        scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(10.0f, 10.0f);
        scoreText.setString("Score: 0");
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Kontrol Lompat / Reset Game
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    if (!gameOver) {
                        velocity = JUMP_STRENGTH;
                    } else {
                        // Reset Game
                        bird.setPosition(80.0f, SCREEN_HEIGHT / 2.0f);
                        velocity = 0.0f;
                        pipes.clear();
                        score = 0;
                        gameOver = false;
                        if (font.loadFromFile("arial.ttf")) scoreText.setString("Score: 0");
                    }
                }
            }
        }

        if (!gameOver) {
            // Update Fisika Burung
            velocity += GRAVITY;
            bird.move(0.0f, velocity);

            // Batas Atas dan Bawah Layar
            if (bird.getPosition().y <= 0 || bird.getPosition().y + bird.getRadius() * 2 >= SCREEN_HEIGHT) {
                gameOver = true;
            }

            // Spawn Pipa Baru
            if (pipeClock.getElapsedTime().asSeconds() > PIPE_SPAWN_TIME) {
                float topHeight = rand() % (SCREEN_HEIGHT - static_cast<int>(GAP_SIZE) - 100) + 50;

                Pipe newPipe;
                // Pipa Atas
                newPipe.topPipe.setSize(sf::Vector2f(50.0f, topHeight));
                newPipe.topPipe.setFillColor(sf::Color::Green);
                newPipe.topPipe.setPosition(SCREEN_WIDTH, 0.0f);

                // Pipa Bawah
                newPipe.bottomPipe.setSize(sf::Vector2f(50.0f, SCREEN_HEIGHT - topHeight - GAP_SIZE));
                newPipe.bottomPipe.setFillColor(sf::Color::Green);
                newPipe.bottomPipe.setPosition(SCREEN_WIDTH, topHeight + GAP_SIZE);

                pipes.push_back(newPipe);
                pipeClock.restart();
            }

            // Perbarui Pergerakan Pipa & Cek Deteksi Tabrakan
            for (size_t i = 0; i < pipes.size(); i++) {
                pipes[i].topPipe.move(-PIPE_SPEED, 0.0f);
                pipes[i].bottomPipe.move(-PIPE_SPEED, 0.0f);

                // Cek Tabrakan dengan Burung
                if (bird.getGlobalBounds().intersects(pipes[i].topPipe.getGlobalBounds()) ||
                    bird.getGlobalBounds().intersects(pipes[i].bottomPipe.getGlobalBounds())) {
                    gameOver = true;
                }

                // Tambah Skor saat Lewat Pipa
                if (!pipes[i].passed && pipes[i].topPipe.getPosition().x + 50.0f < bird.getPosition().x) {
                    pipes[i].passed = true;
                    score++;
                    if (font.loadFromFile("arial.ttf")) {
                        scoreText.setString("Score: " + std::to_string(score));
                    }
                }
            }

            // Hapus Pipa yang Sudah Keluar Layar
            if (!pipes.empty() && pipes[0].topPipe.getPosition().x < -50.0f) {
                pipes.erase(pipes.begin());
            }
        }

        // Render Layar
        window.clear(sf::Color(135, 206, 235)); // Warna Langit (Sky Blue)

        for (const auto& pipe : pipes) {
            window.draw(pipe.topPipe);
            window.draw(pipe.bottomPipe);
        }

        window.draw(bird);

        if (font.loadFromFile("arial.ttf")) {
            window.draw(scoreText);
        }

        window.display();
    }

    return 0;
}
