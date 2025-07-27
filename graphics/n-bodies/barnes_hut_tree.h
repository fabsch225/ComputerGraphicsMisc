//
// Created by fabian on 7/27/25.
//

#pragma once
#include "body.h"
#include "quad.h"

struct BHTree {
    Quad quad;
    Body* body = nullptr;
    bool isExternal = true;

    BHTree* NW = nullptr;
    BHTree* NE = nullptr;
    BHTree* SW = nullptr;
    BHTree* SE = nullptr;

    double totalMass = 0;
    double centerX = 0;
    double centerY = 0;

    explicit BHTree(const Quad &q) : quad(q) {}

    ~BHTree() {
        delete NW; delete NE; delete SW; delete SE;
    }

    void insert(Body* b) {
        if (!body && isExternal) {
            body = b;
            totalMass = b->mass;
            centerX = b->x;
            centerY = b->y;
            return;
        }

        if (isExternal) {
            subdivide();
            insertBody(body);
            body = nullptr;
        }

        insertBody(b);
        updateMassAndCenter(b);
    }

    void updateMassAndCenter(Body* b) {
        totalMass += b->mass;
        centerX = (centerX * (totalMass - b->mass) + b->x * b->mass) / totalMass;
        centerY = (centerY * (totalMass - b->mass) + b->y * b->mass) / totalMass;
    }

    void insertBody(Body* b) {
        if (quad.NW().contains(b->x, b->y)) {
            if (!NW) NW = new BHTree(quad.NW());
            NW->insert(b);
        } else if (quad.NE().contains(b->x, b->y)) {
            if (!NE) NE = new BHTree(quad.NE());
            NE->insert(b);
        } else if (quad.SW().contains(b->x, b->y)) {
            if (!SW) SW = new BHTree(quad.SW());
            SW->insert(b);
        } else if (quad.SE().contains(b->x, b->y)) {
            if (!SE) SE = new BHTree(quad.SE());
            SE->insert(b);
        }
    }

    void updateForce(Body* b, double theta = 0.5) {
        if (!totalMass || b == body) return;

        double dx = centerX - b->x;
        double dy = centerY - b->y;
        double dist = sqrt(dx*dx + dy*dy + 1e-5);
        double s = quad.length;

        if (isExternal || (s / dist < theta)) {
            Body pseudo(centerX, centerY, totalMass);
            b->addForce(pseudo);
        } else {
            if (NW) NW->updateForce(b, theta);
            if (NE) NE->updateForce(b, theta);
            if (SW) SW->updateForce(b, theta);
            if (SE) SE->updateForce(b, theta);
        }
    }

    void subdivide() {
        isExternal = false;
    }
};
