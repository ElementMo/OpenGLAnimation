/**
 * @Author Mobai Wang
 * @Date 2020/12/13
 * @Version 1.0
 */

import processing.core.*;
import java.util.ArrayList;

public class Lab5SPH extends PApplet {

    QTree qtree;
    ArrayList<SPHParticle> sph_particles;
    int tickCount = 30;

    PImage bunny;
    PGraphics particle_img;

    int image_based_particle_color = color(70, 220, 255);
    float image_based_particle_viscocity = 30;
    float image_based_particle_mass = 10;

    public void setup() {
        size(800, 800);
        colorMode(HSB);
        particle_img = createGraphics(width, height);

        bunny = loadImage("bunny2d.jpg");

        sph_particles = new ArrayList<SPHParticle>();

        QRect boundary = new QRect(width * 0.5f, height * 0.5f, width * 0.8f, height * 0.8f);
        qtree = new QTree(boundary, 4);

        PFont pfont = createFont("Arial Bold", 50);
        particle_img.beginDraw();
        particle_img.textFont(pfont);
        particle_img.endDraw();
    }

    public void draw() {
        // Display
        background(0);

        // Instruction
        fill(255);
        textSize(20);
        text("Press 1-5 to spawn different particles", 50, 50);

        // Re-create octree every frame (Slightly larger than the window to prevent
        // popping)
        QRect boundary = new QRect(width * 0.5f, height * 0.5f, width * 0.8f, height * 0.8f);
        qtree = new QTree(boundary, 4);

        for (int i = 0; i < sph_particles.size(); i++) {
            SPHParticle p = sph_particles.get(i);
            QPoint qp = new QPoint(p.position.x, p.position.y, i);
            qtree.insert(qp);
        }
        qtree.show(this);

        for (int count = 0; count < tickCount; count++) {
            for (int i = 0; i < sph_particles.size(); i++) {
                SPHParticle p = sph_particles.get(i);
                // Set the nearby search rect and query nearby particles
                QRect nearby_area = new QRect(p.position.x, p.position.y, 20, 20);
                ArrayList<QPoint> query_particles = new ArrayList<QPoint>();
                query_particles = qtree.query(nearby_area, query_particles);

                ArrayList<SPHParticle> nearby = new ArrayList<SPHParticle>();
                for (int j = 0; j < query_particles.size(); j++) {
                    nearby.add(sph_particles.get(query_particles.get(j).index));
                }
                p.Step(nearby);
                p.Intergrate(width, height, width);
            }
        }

        for (SPHParticle particle : sph_particles) {
            particle.Show(this);
        }

        surface.setTitle("Frame Count: " + frameCount);

        // saveFrame("renderings/intro/#####.png");
    }

    
    public void mousePressed() {
        if (mouseButton == LEFT) {
            SpawnBall();
        }
    }

    // Draw different images and spawn particles

    public void keyPressed() {
        // Keyboard 1 - 5
        int particle_shape_index = key - 49;

        if (particle_shape_index == 0) {
            particle_img.beginDraw();
            particle_img.background(0);
            particle_img.rectMode(CENTER);
            particle_img.textSize(149);

            particle_img.rect(width / 2, height / 2 - 175, 456, 17);
            particle_img.rect(width / 2, height / 2 + 6, 456, 17);

            image_based_particle_color = color(31, 126, 235);
            image_based_particle_viscocity = 10;
            image_based_particle_mass = 50;

            particle_img.endDraw();
            SpawnFromImage(particle_img);

            particle_img.beginDraw();
            particle_img.background(0);
            particle_img.text("GWU", 206, 375);
            image_based_particle_color = color(145, 196, 191);

            particle_img.endDraw();
            SpawnFromImage(particle_img);
        }

        if (particle_shape_index == 1) {
            particle_img.beginDraw();
            particle_img.background(0);
            particle_img.textSize(147);
            particle_img.text("Festival", 125, 294);
            particle_img.text("of", 312, 461);
            particle_img.text("Animation", 19, 602);

            image_based_particle_color = color(122, 65, 255);
            image_based_particle_viscocity = 10;
            image_based_particle_mass = 50;

            particle_img.endDraw();
            SpawnFromImage(particle_img);
        }
        if (particle_shape_index == 2) {
            particle_img.beginDraw();
            particle_img.background(0);
            particle_img.textSize(170);
            particle_img.text("WCSPH", 86, 261);
            particle_img.textSize(134);
            particle_img.text("Fluid", 247, 440);
            particle_img.text("Simulation", 44, 575);
            image_based_particle_color = color(135, 255, 255);
            particle_img.endDraw();
            SpawnFromImage(particle_img);
        }
        if (particle_shape_index == 3) {
            particle_img.beginDraw();
            particle_img.background(0);
            particle_img.textSize(117);
            particle_img.text("Mobai Wang", 28, 370);
            particle_img.text("Fall 2020", 136, 500);
            particle_img.endDraw();
            SpawnFromImage(particle_img);
        }
        if (particle_shape_index == 4) {
            particle_img.beginDraw();
            particle_img.background(0);
            particle_img.textSize(117);
            particle_img.text("Thank You!", 75, 300);
            image_based_particle_color = color(30, 150, 255);
            image_based_particle_viscocity = 100;
            image_based_particle_mass = 20;
            particle_img.endDraw();
            SpawnFromImage(particle_img);

            particle_img.beginDraw();
            particle_img.background(0);
            image_based_particle_color = color(122, 65, 255);
            particle_img.image(bunny, width / 2 - 200, height / 2 + 150 - bunny.height / 2);
            particle_img.endDraw();
            SpawnFromImage(particle_img);
        }

    }

    void SpawnFromImage(PGraphics base_image) {

        int particle_count = sph_particles.size() - 1;
        int particle_gap = 7;
        PImage temp_img = new PImage(base_image.getImage());

        for (int x = 0; x < base_image.width; x += particle_gap) {
            for (int y = 0; y < base_image.height; y += particle_gap) {
                if (temp_img.get(x, y) == color(255)) {
                    SPHParticle sph_p = new SPHParticle(new PVector(x, y), image_based_particle_viscocity,
                            image_based_particle_mass, image_based_particle_color);
                    sph_particles.add(sph_p);
                    QPoint qp = new QPoint(sph_p.position.x, sph_p.position.y, particle_count);
                    qtree.insert(qp);

                    particle_count++;
                }
            }
        }
        println("Spawned Particles From Image");
    }

    void SpawnBall() {
        println("Spawned Ball!");

        int particle_count = sph_particles.size() - 1;
        int ball_r = 50;
        int particle_gap = 7;

        PVector center = new PVector(width / 2, height / 4);

        for (int x = (int) center.x - ball_r; x <= (int) center.x + ball_r; x += particle_gap) {
            for (int y = (int) center.y - ball_r; y <= (int) center.y + ball_r; y += particle_gap) {
                PVector pos = new PVector(x, y);
                if (pos.dist(center) < ball_r) {

                    SPHParticle sph_p = new SPHParticle(pos, 5, 2, color(70, 220, 255));
                    sph_particles.add(sph_p);
                    QPoint qp = new QPoint(sph_p.position.x, sph_p.position.y, particle_count);
                    qtree.insert(qp);

                    particle_count++;
                }
            }
        }
    }

    public void settings() {
        size(800, 800, JAVA2D);
        smooth(8);
    }

    public static void main(String[] args) {
        PApplet.main("Lab5SPH");
    }
}
