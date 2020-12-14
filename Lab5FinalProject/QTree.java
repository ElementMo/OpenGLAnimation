import java.util.ArrayList;
import processing.core.PApplet;

public class QTree {
    QRect boundary;
    ArrayList<QPoint> points;
    int capacity;
    Boolean isDivided;

    QTree northEast, northWest, southEast, southWest;

    QTree(QRect boundary, int n) {
        this.boundary = new QRect(boundary);
        this.capacity = n;
        this.points = new ArrayList<QPoint>();
        this.isDivided = false;
    }

    void subdivide() {
        float x = this.boundary.x;
        float y = this.boundary.y;
        float w = this.boundary.w * 0.5f;
        float h = this.boundary.h * 0.5f;

        QRect ne = new QRect(x + w, y - h, w, h);
        northEast = new QTree(ne, this.capacity);
        QRect nw = new QRect(x - w, y - h, w, h);
        northWest = new QTree(nw, this.capacity);
        QRect se = new QRect(x + w, y + h, w, h);
        southEast = new QTree(se, this.capacity);
        QRect sw = new QRect(x - w, y + h, w, h);
        southWest = new QTree(sw, this.capacity);
        isDivided = true;
    }

    Boolean insert(QPoint point) {
        if (!this.boundary.contains(point)) {
            return false;
        }

        if (points.size() < this.capacity) {
            points.add(point);
            return true;
        } else {
            if (!isDivided) {
                subdivide();
            }
            if (northEast.insert(point)) {
                return true;
            } else if (this.northWest.insert(point)) {
                return true;
            } else if (this.southEast.insert(point)) {
                return true;
            } else if (this.southWest.insert(point)) {
                return true;
            }
        }
        return false;
    }

    ArrayList<QPoint> query(QRect range, ArrayList<QPoint> found) {
        if (found == null) {
            found = new ArrayList<QPoint>();
        }
        if (this.boundary.intersects(range)) {

            for (QPoint p : points) {
                if (range.contains(p)) {
                    found.add(p);
                }
            }
            if (isDivided) {
                northWest.query(range, found);
                northEast.query(range, found);
                southWest.query(range, found);
                southEast.query(range, found);
            }
        }
        return found;
    }

    void Clear() {
        points.clear();
        if (isDivided) {
            northWest.Clear();
            northEast.Clear();
            southWest.Clear();
            southEast.Clear();
        }
    }

    void show(PApplet p) {
        p.stroke(100);
        p.strokeWeight(0.5f);
        p.noFill();
        p.rectMode(PApplet.CENTER);
        p.rect(boundary.x, boundary.y, boundary.w * 2.0f, boundary.h * 2.0f);
        if (isDivided) {
            this.northEast.show(p);
            this.northWest.show(p);
            this.southEast.show(p);
            this.southWest.show(p);
        }
    }
}