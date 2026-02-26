#pragma once
#include "pch.hpp"

namespace CameraManager
{
    class CameraController
    {
    public:
        enum CameraState
        {
            STATIC,      // Camera doesn't move
            INPUT,       // Camera controlled by WASD input
            FOLLOW       // Camera follows a target
        };

        CameraController();

        void Init();
        void Update(float dt);
        void SetPosition(const AEVec2& pos);
        void SetBounds(float minX, float maxX, float minY, float maxY);
        void SetState(CameraState newState);
        void SetFollowTarget(const AEVec2* target);
        void SetFollowSpeed(float speed);

		// ===== GETTERS =====
        AEVec2 GetPosition() const;
        CameraState GetState() const;
		std::string PrintState() const;

        void SetMoveSpeed(float speed);

        // ===== ZOOM FUNCTIONS =====
        float GetZoom() const;
        void  SetZoom(float zoom);
        void  SetZoomSpeed(float speed);
        void  SetZoomBounds(float minZoom, float maxZoom);
        void  ZoomIn(float dt);
        void  ZoomOut(float dt);

    private:
        AEVec2 position;
        float moveSpeed;
        float followSpeed;

        // Boundary limits
        float boundMinX;
        float boundMaxX;
        float boundMinY;
        float boundMaxY;
        bool useBounds;

        // Camera state
        CameraState currentState;
        const AEVec2* followTarget;

		// Zoom Members
        float zoomLevel;      
        float zoomSpeed;      
        float zoomMin;        
        float zoomMax;        
    };

    // Global camera instance
    extern CameraController camera;
}