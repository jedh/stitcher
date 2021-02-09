# Stitcher

A sublevel streaming and stitching solution for UE4. This is ideal for scrolling games have lots of sublevels or "chunks". This repo contains the plugin and a containing project used for testing and development of features that don't need to be exported with the plugin. The main idea is to leverage screen edge detection to more intelligently load sublevels compared to using [Level Streaming Volumes](https://docs.unrealengine.com/en-US/BuildingWorlds/LevelStreaming/StreamingVolumes/index.html), which are designed to work better with a camera position in first-person or close third-person.

This solution is also designed to be data-driven via UDataAssets - after a small bit of setup with collision and trace channels, all you need to do is define your sublevel arrays in UDataAssets based on **UChunkContainerDataAsset**.

Currently a single copy of a sublevel is soft-referenced in memory and instantiated once when encountered the first time in game, it will be streamed instead on all subsequent encounters. This will be expanded on for different loading and referencing behaviours in the future.
