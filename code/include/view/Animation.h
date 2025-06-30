#ifndef ANIMATION_H
#define ANIMATION_H

class Animation {
public:
    Animation(const QList<QString>& frameNames, double frameRate = 10.0, bool loops = true);
    void update(double deltaTime);
    const QString& getCurrentFrameName() const;
    void reset();
    bool isFinished() const;

private:
    QList<QString> m_frameNames; 

    bool m_loops;                 
    double m_frameDuration;       
    double m_elapsedTime;         
    int m_currentIndex;           
};

#endif // ANIMATION_H