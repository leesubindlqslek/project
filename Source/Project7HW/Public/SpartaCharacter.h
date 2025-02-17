#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class PROJECT7HW_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// �⺻ ������
	ASpartaCharacter();

	// ī�޶� �� ������ �� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	// ü�� UI ���� ������Ʈ (��: ü�� ǥ�� UI)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	// ü�� ���� �Լ���
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	// ������ ó�� �� ü�� ����
	virtual float TakeDamage(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

protected:
	// �ִ� ü�� (����Ʈ 100)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.f;
	  
	// ���� ü��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	// �ʱ�ȭ �� �÷��� ���� �� ó��
	virtual void BeginPlay() override;

	// �÷��̾� �Է� ó��
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ����, �̵� ��
	UFUNCTION()
	void Move(const FInputActionValue& value);

	UFUNCTION()
	void StartJump(const FInputActionValue& value);

	UFUNCTION()
	void StopJump(const FInputActionValue& value);

	UFUNCTION()
	void Look(const FInputActionValue& value);

	UFUNCTION()
	void StartSprint(const FInputActionValue& value);

	UFUNCTION()
	void StopSprint(const FInputActionValue& value);

	// ��� ó��
	void OnDeath();

	// ü�� UI ������Ʈ
	void UpdateOverheadHP();

private:
	// ������Ʈ ���� ����
	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
};
