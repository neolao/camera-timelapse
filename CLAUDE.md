# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Language

All repository content — code, comments, documentation, and OpenSpec artifacts — MUST be written in English.

## Project

Autonomous camera timelapse firmware for the AI-Thinker ESP32-CAM-MB board (OV2640 camera, SD slot, CH340 USB programmer). The project is in its early stages — no application code exists yet.

## Development Workflow

This project uses **OpenSpec** (spec-first development). All changes go through a structured design phase before implementation.

Available commands:
- `/opsx:propose` — Propose a new change (generates proposal, design, tasks)
- `/opsx:apply` — Implement tasks from a change
- `/opsx:explore` — Explore an idea or investigate a problem
- `/opsx:archive` — Archive a completed change

Changes are stored in `openspec/changes/<change-name>/` with their artifacts (`proposal.md`, `design.md`, `tasks.md`).

## OpenSpec Configuration

`openspec/config.yaml` holds project context (tech stack, conventions) and per-artifact rules. Update it as the project evolves.
