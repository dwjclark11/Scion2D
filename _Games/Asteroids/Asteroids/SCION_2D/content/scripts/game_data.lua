--[[
Copyright 2025 @JADE-ite Games Studios. All rights reserved 
--]]

GameData = S2D_Class("GameData")

function GameData:Init()
	self.numAsteroids = 0
	self.score = 0
	self.shipHandler = gShipHandler
end

function GameData:IsGameOver()
	return not self.shipHandler:ShipsAvailable() 
end 

function GameData:AddToScore(value)
	self.score = S2D_clamp(self.score + value, 0, 99999)
end

function GameData:GetScore()
	return self.score
end

function GameData:Reset() 
	self.numAsteroids = 0
	self.score = 0
end

gData = GameData:Create()
