#ifndef CORECOMPONENT_H
#define CORECOMPONENT_H

class CoreComponent {
public:
	CoreComponent() = default;
	CoreComponent(const CoreComponent&) = delete;
	CoreComponent(CoreComponent&&) = delete;
	CoreComponent& operator=(const CoreComponent&) = delete;
	CoreComponent& operator=(CoreComponent&&) = delete;
	virtual ~CoreComponent() = default;

	virtual void Update(const unsigned long long&) = 0;
};

#endif