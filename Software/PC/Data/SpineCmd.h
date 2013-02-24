#pragma once

class SpineCmd
{
public:
	SpineCmd();
	virtual ~SpineCmd(void);

	enum	KickStrength { Normal, Strong };
	
	void	Stop();
	virtual void	SetSpeed(double absSpeed, double radAngle, double radTurnRate, double ramp = false);

	double	GetAbsSpeed(void)		const { return absSpeed_; }
	double	GetRadAngle(void)		const { return radAngle_; }
	double	GetRadTurnRate(void)	const { return radTurnRate_; }

	int*	GetWheelSpeeds(void) const;

	static const unsigned NormalKick = 3;
	static const unsigned StrongKick = 7;

	void	Kick();
	void	KickRegulated(KickStrength strength = Normal);

	void	ClearKick(void);
	bool	KickActive()			const { return kick_; }

	bool    NewCommands (void)		const;
	void	ClearCommands (void);

	virtual void	Copy(const SpineCmd *cmd);

	static const double MaxSpeed;
	static const double MaxTurnRate;

protected:
	double	absSpeed_;
	double	radAngle_ ;
	double	radTurnRate_;

	bool	kick_;
	int		kickTime_;

	bool	newSpeedData_;
};

